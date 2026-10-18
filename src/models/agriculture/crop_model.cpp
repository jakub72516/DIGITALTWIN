#include "models/agriculture/crop_model.hpp"

#include <cmath>
#include <algorithm>

namespace etwin::models::agriculture {

ModelMetadata CropModel::metadata() const {
    ModelMetadata m;
    m.name = "crop_model";
    m.version = "1.0";
    m.description = "WOFOST-style simplified crop growth model";
    m.equations_or_methods = {
        "Biomass accumulation via radiation use efficiency",
        "Growing degree day phenology",
        "Water and nitrogen stress factors",
    };
    m.limitations = {
        "Single crop type (parameterized)",
        "No pest or disease damage in base mode",
    };
    m.valid_conditions = {
        "Annual crops",
        "Daily timesteps over growing season",
    };
    m.known_failure_modes = {
        "Overestimates yield without adequate water stress",
    };
    m.calibration_procedure = "Calibrate RUE and harvest index against field trials";
    m.dependencies = {"atmosphere", "hydrology", "soil"};
    return m;
}

std::vector<FieldRequirement> CropModel::requirements() const {
    return {
        {"temperature", etwin::core::units::kelvin, "Air temperature", true, 0.0, etwin::core::Nanoseconds(0)},
        {"solar_radiation", etwin::core::units::watt_per_m2, "Solar radiation", true, 0.0, etwin::core::Nanoseconds(0)},
        {"soil_moisture", etwin::core::units::dimensionless, "Soil moisture", true, 0.0, etwin::core::Nanoseconds(0)},
    };
}

std::vector<FieldProduct> CropModel::products() const {
    return {
        {"crop_yield", etwin::core::units::kilogram_per_m2, etwin::state::FieldKind::Simulation, "Crop yield"},
        {"biomass", etwin::core::units::kilogram_per_m2, etwin::state::FieldKind::Simulation, "Above-ground biomass"},
        {"leaf_area_index", etwin::core::units::dimensionless, etwin::state::FieldKind::Simulation, "Leaf area index"},
    };
}

void CropModel::initialize(const ExecutionContext& ctx) {
    state_.biomass = 0.0;
    state_.leaf_area_index = 0.0;
    state_.accumulated_gdd = 0.0;
    state_.yield = 0.0;
    state_.mature = false;
    (void)ctx;
}

void CropModel::step(const ExecutionContext& ctx) {
    if (state_.mature) return;

    double dt = static_cast<double>(ctx.time_step.count()) / 86400e9;
    double temp_c = ctx.parameters.contains("temperature")
        ? ctx.parameters.at("temperature") - 273.15
        : 20.0;

    double base = parameters_.at("base_temperature");
    state_.accumulated_gdd += std::max(0.0, temp_c - base) * dt;

    double gdd_maturity = parameters_.at("gdd_to_maturity");
    if (state_.accumulated_gdd >= gdd_maturity) {
        state_.mature = true;
        state_.yield = state_.biomass * parameters_.at("harvest_index");
        return;
    }

    double rue = parameters_.at("radiation_use_efficiency");
    double radiation = ctx.parameters.contains("solar_radiation")
        ? ctx.parameters.at("solar_radiation")
        : 200.0;
    double intercepted = radiation * (1.0 - std::exp(-0.65 * state_.leaf_area_index));
    state_.biomass += rue * intercepted * dt * 1e-3;
    state_.leaf_area_index = std::min(6.0, state_.biomass * 0.002);
}

void CropModel::finalize(const ExecutionContext& ctx) {
    if (!state_.mature) {
        state_.yield = state_.biomass * parameters_.at("harvest_index");
    }
    (void)ctx;
}

void CropModel::validate_inputs(const etwin::state::EarthStateQuery& query) const {
    (void)query;
}

void CropModel::calibrate(const std::map<std::string, std::vector<double>>& calibration_data) {
    (void)calibration_data;
}

std::map<std::string, etwin::state::Field> CropModel::output() const {
    std::map<std::string, etwin::state::Field> result;

    etwin::state::Field yield;
    yield.metadata.name = "crop_yield";
    yield.metadata.unit = etwin::core::units::kilogram_per_m2;
    yield.metadata.kind = etwin::state::FieldKind::Simulation;
    yield.data = {state_.yield};
    yield.metadata.shape = {1};
    result["crop_yield"] = yield;

    etwin::state::Field biomass;
    biomass.metadata.name = "biomass";
    biomass.metadata.unit = etwin::core::units::kilogram_per_m2;
    biomass.metadata.kind = etwin::state::FieldKind::Simulation;
    biomass.data = {state_.biomass};
    biomass.metadata.shape = {1};
    result["biomass"] = biomass;

    return result;
}

void CropModel::reset() {
    state_ = State{};
}

void CropModel::set_parameter(const std::string& name, double value) {
    parameters_[name] = value;
}

} // namespace etwin::models::agriculture
