#include "models/cryosphere/degree_day.hpp"

#include <cmath>
#include <algorithm>

namespace etwin::models::cryosphere {

ModelMetadata DegreeDay::metadata() const {
    ModelMetadata m;
    m.name = "degree_day";
    m.version = "1.0";
    m.description = "Degree-day snow and ice melt model";
    m.equations_or_methods = {
        "Melt = DDF * max(T - T_threshold, 0)",
        "Snow water equivalent accumulation from precipitation",
        "Albedo feedback on melt energy",
    };
    m.limitations = {
        "Temperature-index only (no energy balance)",
        "No refreezing",
    };
    m.valid_conditions = {
        "Glacier and seasonal snow melt",
        "Daily or sub-daily timesteps",
    };
    m.known_failure_modes = {
        "Overestimates melt in windy, dry conditions",
    };
    m.calibration_procedure = "Calibrate DDF against observed glacier mass balance";
    m.dependencies = {"atmosphere"};
    return m;
}

std::vector<FieldRequirement> DegreeDay::requirements() const {
    return {
        {"air_temperature", etwin::core::units::kelvin, "Near-surface air temperature", true, 0.0, etwin::core::Nanoseconds(0)},
        {"precipitation", etwin::core::units::kilogram_per_m2_s, "Precipitation rate", true, 0.0, etwin::core::Nanoseconds(0)},
    };
}

std::vector<FieldProduct> DegreeDay::products() const {
    return {
        {"snow_water_equivalent", etwin::core::units::kilogram_per_m2, etwin::state::FieldKind::Simulation, "Snow water equivalent"},
        {"glacier_mass_balance", etwin::core::units::kilogram_per_m2, etwin::state::FieldKind::Simulation, "Glacier mass balance"},
    };
}

void DegreeDay::initialize(const ExecutionContext& ctx) {
    state_.snow_water_equivalent = 0.0;
    state_.ice_thickness = 100.0;
    state_.albedo = parameters_.at("albedo_ice");
    (void)ctx;
}

void DegreeDay::step(const ExecutionContext& ctx) {
    double dt_days = static_cast<double>(ctx.time_step.count()) / 86400e9;

    double temp_c = 0.0;
    double ddf_snow = parameters_.at("degree_day_factor_snow");
    double ddf_ice = parameters_.at("degree_day_factor_ice");
    double threshold = parameters_.at("melt_threshold");

    if (temp_c > threshold) {
        double snow_melt = ddf_snow * (temp_c - threshold) * dt_days;
        state_.snow_water_equivalent = std::max(0.0, state_.snow_water_equivalent - snow_melt);
        if (state_.snow_water_equivalent == 0.0) {
            state_.ice_thickness -= ddf_ice * (temp_c - threshold) * dt_days / 1000.0;
        }
    }
}

void DegreeDay::finalize(const ExecutionContext& ctx) {
    (void)ctx;
}

void DegreeDay::validate_inputs(const etwin::state::EarthStateQuery& query) const {
    (void)query;
}

void DegreeDay::calibrate(const std::map<std::string, std::vector<double>>& calibration_data) {
    (void)calibration_data;
}

std::map<std::string, etwin::state::Field> DegreeDay::output() const {
    std::map<std::string, etwin::state::Field> result;

    etwin::state::Field swe;
    swe.metadata.name = "snow_water_equivalent";
    swe.metadata.unit = etwin::core::units::kilogram_per_m2;
    swe.metadata.kind = etwin::state::FieldKind::Simulation;
    swe.data = {state_.snow_water_equivalent};
    swe.metadata.shape = {1};
    result["snow_water_equivalent"] = swe;

    return result;
}

void DegreeDay::reset() {
    state_ = State{};
}

void DegreeDay::set_parameter(const std::string& name, double value) {
    parameters_[name] = value;
}

} // namespace etwin::models::cryosphere
