#include "models/wildfire/fire_model.hpp"

#include <cmath>

namespace etwin::models::wildfire {

ModelMetadata FireModel::metadata() const {
    ModelMetadata m;
    m.name = "fire_model";
    m.version = "1.0";
    m.description = "Fire weather index and Rothermel-style spread model";
    m.equations_or_methods = {
        "Fire Weather Index from temperature, humidity, wind",
        "Rate of spread from fuel and weather",
        "Emissions from burned fuel",
    };
    m.limitations = {
        "No fire suppression",
        "No spotting",
    };
    m.valid_conditions = {
        "Wildland fires",
        "Regional burned area estimates",
    };
    m.known_failure_modes = {
        "Underestimates extreme fire behavior",
    };
    m.calibration_procedure = "Calibrate spread coefficient against fire perimeters";
    m.dependencies = {"atmosphere", "ecosystem", "forestry"};
    return m;
}

std::vector<FieldRequirement> FireModel::requirements() const {
    return {
        {"temperature", etwin::core::units::kelvin, "Air temperature", true, 0.0, etwin::core::Nanoseconds(0)},
        {"humidity", etwin::core::units::dimensionless, "Relative humidity", true, 0.0, etwin::core::Nanoseconds(0)},
        {"wind_speed", etwin::core::units::meter_per_second, "Wind speed", true, 0.0, etwin::core::Nanoseconds(0)},
        {"fuel_load", etwin::core::units::kilogram_per_m2, "Available fuel load", true, 0.0, etwin::core::Nanoseconds(0)},
    };
}

std::vector<FieldProduct> FireModel::products() const {
    return {
        {"fire_weather_index", etwin::core::units::dimensionless, etwin::state::FieldKind::Simulation, "Fire weather index"},
        {"burned_area", etwin::core::units::dimensionless, etwin::state::FieldKind::Simulation, "Burned area"},
        {"fire_emissions", etwin::core::units::kilogram_per_m2, etwin::state::FieldKind::Simulation, "Carbon emissions from fire"},
    };
}

void FireModel::initialize(const ExecutionContext& ctx) {
    state_.fire_weather_index = 5.0;
    state_.burned_area = 0.0;
    state_.fuel_moisture = 15.0;
    state_.emissions = 0.0;
    (void)ctx;
}

void FireModel::step(const ExecutionContext& ctx) {
    (void)ctx;
}

void FireModel::finalize(const ExecutionContext& ctx) {
    (void)ctx;
}

void FireModel::validate_inputs(const etwin::state::EarthStateQuery& query) const {
    (void)query;
}

void FireModel::calibrate(const std::map<std::string, std::vector<double>>& calibration_data) {
    (void)calibration_data;
}

std::map<std::string, etwin::state::Field> FireModel::output() const {
    std::map<std::string, etwin::state::Field> result;

    etwin::state::Field fwi;
    fwi.metadata.name = "fire_weather_index";
    fwi.metadata.unit = etwin::core::units::dimensionless;
    fwi.metadata.kind = etwin::state::FieldKind::Simulation;
    fwi.data = {state_.fire_weather_index};
    fwi.metadata.shape = {1};
    result["fire_weather_index"] = fwi;

    etwin::state::Field ba;
    ba.metadata.name = "burned_area";
    ba.metadata.unit = etwin::core::units::dimensionless;
    ba.metadata.kind = etwin::state::FieldKind::Simulation;
    ba.data = {state_.burned_area};
    ba.metadata.shape = {1};
    result["burned_area"] = ba;

    return result;
}

void FireModel::reset() {
    state_ = State{};
}

void FireModel::set_parameter(const std::string& name, double value) {
    parameters_[name] = value;
}

} // namespace etwin::models::wildfire
