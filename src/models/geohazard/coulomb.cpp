#include "models/geohazard/coulomb.hpp"

#include <cmath>

namespace etwin::models::geohazard {

ModelMetadata Coulomb::metadata() const {
    ModelMetadata m;
    m.name = "coulomb";
    m.version = "1.0";
    m.description = "Coulomb stress change model for seismic triggering";
    m.equations_or_methods = {
        "Coulomb failure stress: CFS = tau + mu * (sigma - p)",
        "Static stress transfer from source to receiver faults",
    };
    m.limitations = {
        "Static stress only (no dynamic triggering)",
        "Requires fault geometry assumptions",
    };
    m.valid_conditions = {
        "Aftershock triggering analysis",
        "Retrospective catalog evaluation",
    };
    m.known_failure_modes = {
        "Sensitivity to unknown fault friction",
    };
    m.calibration_procedure = "Fit friction coefficient against aftershock spatial distribution";
    m.dependencies = {"seismic"};
    return m;
}

std::vector<FieldRequirement> Coulomb::requirements() const {
    return {
        {"fault_geometry", etwin::core::units::dimensionless, "Fault orientation and geometry", true, 0.0, etwin::core::Nanoseconds(0)},
        {"stress_state", etwin::core::units::pascal, "Background stress tensor", true, 0.0, etwin::core::Nanoseconds(0)},
    };
}

std::vector<FieldProduct> Coulomb::products() const {
    return {
        {"coulomb_stress_change", etwin::core::units::pascal, etwin::state::FieldKind::Simulation, "Coulomb stress change"},
        {"aftershock_probability", etwin::core::units::dimensionless, etwin::state::FieldKind::Simulation, "Aftershock probability"},
    };
}

void Coulomb::initialize(const ExecutionContext& ctx) {
    state_.shear_stress = 0.0;
    state_.normal_stress = 100e6;
    state_.pore_pressure = 30e6;
    (void)ctx;
}

void Coulomb::step(const ExecutionContext& ctx) {
    (void)ctx;
}

void Coulomb::finalize(const ExecutionContext& ctx) {
    (void)ctx;
}

void Coulomb::validate_inputs(const etwin::state::EarthStateQuery& query) const {
    (void)query;
}

void Coulomb::calibrate(const std::map<std::string, std::vector<double>>& calibration_data) {
    (void)calibration_data;
}

double coulomb_failure_stress(double tau, double sigma, double p, double mu, double cohesion) {
    (void)cohesion;
    return tau + mu * (sigma - p);
}

std::map<std::string, etwin::state::Field> Coulomb::output() const {
    std::map<std::string, etwin::state::Field> result;

    double cfs = coulomb_failure_stress(
        state_.shear_stress,
        state_.normal_stress,
        state_.pore_pressure,
        parameters_.at("friction_coefficient"),
        parameters_.at("cohesion"));

    etwin::state::Field f;
    f.metadata.name = "coulomb_stress_change";
    f.metadata.unit = etwin::core::units::pascal;
    f.metadata.kind = etwin::state::FieldKind::Simulation;
    f.data = {cfs};
    f.metadata.shape = {1};
    result["coulomb_stress_change"] = f;

    return result;
}

void Coulomb::reset() {
    state_ = State{};
}

void Coulomb::set_parameter(const std::string& name, double value) {
    parameters_[name] = value;
}

} // namespace etwin::models::geohazard
