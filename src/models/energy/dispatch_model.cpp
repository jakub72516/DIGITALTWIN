#include "models/energy/dispatch_model.hpp"

#include <cmath>
#include <algorithm>

namespace etwin::models::energy {

ModelMetadata DispatchModel::metadata() const {
    ModelMetadata m;
    m.name = "dispatch_model";
    m.version = "1.0";
    m.description = "Simplified unit-commitment electricity dispatch model";
    m.equations_or_methods = {
        "Merit-order dispatch: renewable first, then fossil",
        "Storage charge/discharge balancing",
        "Emissions from fossil generation",
    };
    m.limitations = {
        "Single zone (no transmission)",
        "No unit-level detail",
    };
    m.valid_conditions = {
        "Grid zone with aggregate demand",
    };
    m.known_failure_modes = {
        "Underestimates cost without transmission constraints",
    };
    m.calibration_procedure = "Calibrate capacity factors against grid operator data";
    m.dependencies = {"atmosphere", "population", "infrastructure"};
    return m;
}

std::vector<FieldRequirement> DispatchModel::requirements() const {
    return {
        {"demand", etwin::core::units::watt_per_m2, "Electricity demand", true, 0.0, etwin::core::Nanoseconds(0)},
        {"solar_radiation", etwin::core::units::watt_per_m2, "Solar radiation", true, 0.0, etwin::core::Nanoseconds(0)},
        {"wind_speed", etwin::core::units::meter_per_second, "Wind speed", true, 0.0, etwin::core::Nanoseconds(0)},
    };
}

std::vector<FieldProduct> DispatchModel::products() const {
    return {
        {"renewable_generation", etwin::core::units::watt_per_m2, etwin::state::FieldKind::Simulation, "Renewable generation"},
        {"fossil_generation", etwin::core::units::watt_per_m2, etwin::state::FieldKind::Simulation, "Fossil generation"},
        {"energy_emissions", etwin::core::units::kilogram_per_m2_s, etwin::state::FieldKind::Simulation, "Carbon emissions from electricity"},
        {"curtailment", etwin::core::units::watt_per_m2, etwin::state::FieldKind::Simulation, "Renewable curtailment"},
    };
}

void DispatchModel::initialize(const ExecutionContext& ctx) {
    state_.demand = ctx.parameters.contains("demand") ? ctx.parameters.at("demand") : 800.0;
    state_.renewable_generation = 0.0;
    state_.fossil_generation = 0.0;
    state_.storage_charge = 0.0;
    state_.emissions = 0.0;
    state_.curtailment = 0.0;
}

void DispatchModel::step(const ExecutionContext& ctx) {
    double renewable_capacity = parameters_.at("renewable_capacity");
    double fossil_capacity = parameters_.at("fossil_capacity");
    double capacity_factor = parameters_.at("renewable_capacity_factor");

    state_.demand = ctx.parameters.contains("demand") ? ctx.parameters.at("demand") : state_.demand;

    double renewable_available = renewable_capacity * capacity_factor;
    state_.renewable_generation = std::min(renewable_available, state_.demand);

    double remaining = state_.demand - state_.renewable_generation;
    state_.fossil_generation = std::min(fossil_capacity, remaining);

    double unmet = remaining - state_.fossil_generation;
    state_.curtailment = std::max(0.0, renewable_available - state_.renewable_generation);

    state_.emissions = state_.fossil_generation * parameters_.at("emission_intensity_fossil") * 1e-3;
}

void DispatchModel::finalize(const ExecutionContext& ctx) {
    (void)ctx;
}

void DispatchModel::validate_inputs(const etwin::state::EarthStateQuery& query) const {
    (void)query;
}

void DispatchModel::calibrate(const std::map<std::string, std::vector<double>>& calibration_data) {
    (void)calibration_data;
}

std::map<std::string, etwin::state::Field> DispatchModel::output() const {
    std::map<std::string, etwin::state::Field> result;

    etwin::state::Field rg;
    rg.metadata.name = "renewable_generation";
    rg.metadata.unit = etwin::core::units::watt_per_m2;
    rg.metadata.kind = etwin::state::FieldKind::Simulation;
    rg.data = {state_.renewable_generation};
    rg.metadata.shape = {1};
    result["renewable_generation"] = rg;

    etwin::state::Field em;
    em.metadata.name = "energy_emissions";
    em.metadata.unit = etwin::core::units::kilogram_per_m2_s;
    em.metadata.kind = etwin::state::FieldKind::Simulation;
    em.data = {state_.emissions};
    em.metadata.shape = {1};
    result["energy_emissions"] = em;

    return result;
}

void DispatchModel::reset() {
    state_ = State{};
}

void DispatchModel::set_parameter(const std::string& name, double value) {
    parameters_[name] = value;
}

} // namespace etwin::models::energy
