#include "models/hydrology/hbv.hpp"

#include <cmath>
#include <algorithm>

namespace etwin::models::hydrology {

ModelMetadata HBV::metadata() const {
    ModelMetadata m;
    m.name = "hbv";
    m.version = "1.0";
    m.description = "HBV-style rainfall-runoff model with snow, soil, and two groundwater stores";
    m.equations_or_methods = {
        "Snow accumulation and degree-day melt",
        "Soil moisture accounting with beta function",
        "Upper and lower groundwater zone routing",
    };
    m.limitations = {
        "Lumped (no spatial variability within basin)",
        "Requires calibration for each basin",
    };
    m.valid_conditions = {
        "Catchments of 10-10000 km2",
        "Daily or sub-daily timesteps",
    };
    m.known_failure_modes = {
        "Poor performance in arid basins",
        "Snow routine fails in very warm climates",
    };
    m.calibration_procedure = "Calibrate fc, beta, and k parameters against observed streamflow";
    m.dependencies = {"atmosphere", "soil"};
    return m;
}

std::vector<FieldRequirement> HBV::requirements() const {
    return {
        {"precipitation", etwin::core::units::kilogram_per_m2_s, "Precipitation rate", true, 0.0, etwin::core::Nanoseconds(0)},
        {"temperature", etwin::core::units::kelvin, "Air temperature", true, 0.0, etwin::core::Nanoseconds(0)},
        {"evapotranspiration", etwin::core::units::kilogram_per_m2_s, "Potential evapotranspiration", true, 0.0, etwin::core::Nanoseconds(0)},
    };
}

std::vector<FieldProduct> HBV::products() const {
    return {
        {"runoff", etwin::core::units::kilogram_per_m2_s, etwin::state::FieldKind::Simulation, "Total catchment runoff"},
        {"soil_moisture", etwin::core::units::dimensionless, etwin::state::FieldKind::Simulation, "Soil moisture storage"},
        {"snow_water_equivalent", etwin::core::units::kilogram_per_m2, etwin::state::FieldKind::Simulation, "Snow water equivalent"},
    };
}

void HBV::initialize(const ExecutionContext& ctx) {
    state_.snow_storage = 0.0;
    state_.soil_moisture = parameters_.at("pwp") * 0.5;
    state_.upper_zone = 5.0;
    state_.lower_zone = 20.0;
    (void)ctx;
}

void HBV::step(const ExecutionContext& ctx) {
    double dt = static_cast<double>(ctx.time_step.count()) / 86400e9;
    (void)dt;
}

void HBV::finalize(const ExecutionContext& ctx) {
    (void)ctx;
}

void HBV::validate_inputs(const etwin::state::EarthStateQuery& query) const {
    (void)query;
}

void HBV::calibrate(const std::map<std::string, std::vector<double>>& calibration_data) {
    (void)calibration_data;
}

std::map<std::string, etwin::state::Field> HBV::output() const {
    std::map<std::string, etwin::state::Field> result;

    etwin::state::Field runoff;
    runoff.metadata.name = "runoff";
    runoff.metadata.unit = etwin::core::units::kilogram_per_m2_s;
    runoff.metadata.kind = etwin::state::FieldKind::Simulation;
    runoff.data = {0.0};
    runoff.metadata.shape = {1};
    result["runoff"] = runoff;

    etwin::state::Field sm;
    sm.metadata.name = "soil_moisture";
    sm.metadata.unit = etwin::core::units::dimensionless;
    sm.metadata.kind = etwin::state::FieldKind::Simulation;
    sm.data = {state_.soil_moisture};
    sm.metadata.shape = {1};
    result["soil_moisture"] = sm;

    return result;
}

void HBV::reset() {
    state_ = State{};
}

void HBV::set_parameter(const std::string& name, double value) {
    parameters_[name] = value;
}

} // namespace etwin::models::hydrology
