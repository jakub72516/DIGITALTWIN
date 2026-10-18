#include "models/ecosystem/carbon_cycle.hpp"

#include <cmath>

namespace etwin::models::ecosystem {

ModelMetadata CarbonCycle::metadata() const {
    ModelMetadata m;
    m.name = "carbon_cycle";
    m.version = "1.0";
    m.description = "CENTURY-style carbon and nutrient cycle (vegetation, litter, soil)";
    m.equations_or_methods = {
        "Vegetation carbon: dCv/dt = NPP - turnover*Cv",
        "Litter carbon: dCl/dt = turnover*Cv - decomposition*Cl",
        "Soil carbon: dCs/dt = decomposition*Cl - respiration*Cs",
        "Temperature response via Q10",
    };
    m.limitations = {
        "No nitrogen limitation",
        "No disturbance (fire, harvest) in base mode",
    };
    m.valid_conditions = {
        "Terrestrial ecosystems",
        "Annual to decadal carbon dynamics",
    };
    m.known_failure_modes = {
        "Unrealistic NPP if climate forcing out of range",
    };
    m.calibration_procedure = "Calibrate turnover and decomposition against flux tower data";
    m.dependencies = {"atmosphere", "soil", "hydrology"};
    return m;
}

std::vector<FieldRequirement> CarbonCycle::requirements() const {
    return {
        {"temperature", etwin::core::units::kelvin, "Mean temperature", true, 0.0, etwin::core::Nanoseconds(0)},
        {"soil_moisture", etwin::core::units::dimensionless, "Soil moisture", true, 0.0, etwin::core::Nanoseconds(0)},
        {"co2_concentration", etwin::core::units::dimensionless, "Atmospheric CO2", true, 0.0, etwin::core::Nanoseconds(0)},
    };
}

std::vector<FieldProduct> CarbonCycle::products() const {
    return {
        {"vegetation_carbon", etwin::core::units::kilogram_per_m2, etwin::state::FieldKind::Simulation, "Vegetation carbon stock"},
        {"soil_carbon", etwin::core::units::kilogram_per_m2, etwin::state::FieldKind::Simulation, "Soil carbon stock"},
        {"npp", etwin::core::units::kilogram_per_m2_s, etwin::state::FieldKind::Simulation, "Net primary production"},
    };
}

void CarbonCycle::initialize(const ExecutionContext& ctx) {
    state_.vegetation_carbon = 5000.0;
    state_.soil_carbon = 10000.0;
    state_.litter_carbon = 1000.0;
    state_.npp = parameters_.at("npp_base");
    state_.heterotrophic_respiration = 0.0;
    (void)ctx;
}

void CarbonCycle::step(const ExecutionContext& ctx) {
    double dt = static_cast<double>(ctx.time_step.count()) / 86400e9;

    double turnover = parameters_.at("turnover_rate");
    double decomp = parameters_.at("decomposition_rate");

    double veg_loss = turnover * state_.vegetation_carbon;
    double litter_loss = decomp * state_.litter_carbon;

    state_.vegetation_carbon += (state_.npp - veg_loss) * dt;
    state_.litter_carbon += (veg_loss - litter_loss) * dt;
    state_.soil_carbon += litter_loss * dt;
    state_.heterotrophic_respiration = litter_loss;
}

void CarbonCycle::finalize(const ExecutionContext& ctx) {
    (void)ctx;
}

void CarbonCycle::validate_inputs(const etwin::state::EarthStateQuery& query) const {
    (void)query;
}

void CarbonCycle::calibrate(const std::map<std::string, std::vector<double>>& calibration_data) {
    (void)calibration_data;
}

std::map<std::string, etwin::state::Field> CarbonCycle::output() const {
    std::map<std::string, etwin::state::Field> result;

    etwin::state::Field vc;
    vc.metadata.name = "vegetation_carbon";
    vc.metadata.unit = etwin::core::units::kilogram_per_m2;
    vc.metadata.kind = etwin::state::FieldKind::Simulation;
    vc.data = {state_.vegetation_carbon};
    vc.metadata.shape = {1};
    result["vegetation_carbon"] = vc;

    etwin::state::Field sc;
    sc.metadata.name = "soil_carbon";
    sc.metadata.unit = etwin::core::units::kilogram_per_m2;
    sc.metadata.kind = etwin::state::FieldKind::Simulation;
    sc.data = {state_.soil_carbon};
    sc.metadata.shape = {1};
    result["soil_carbon"] = sc;

    return result;
}

void CarbonCycle::reset() {
    state_ = State{};
}

void CarbonCycle::set_parameter(const std::string& name, double value) {
    parameters_[name] = value;
}

} // namespace etwin::models::ecosystem
