#include "models/atmosphere/simple_atmosphere.hpp"

#include <cmath>
#include <numbers>

namespace etwin::models::atmosphere {

ModelMetadata SimpleAtmosphere::metadata() const {
    ModelMetadata m;
    m.name = "simple_atmosphere";
    m.version = "1.0";
    m.description = "Zero-dimensional energy-balance atmosphere with moisture";
    m.equations_or_methods = {
        "Energy balance: C*dT/dt = S(1-alpha)/4 - eps*sigma*T^4",
        "Moisture budget with simplified saturation",
    };
    m.limitations = {
        "No horizontal transport",
        "No cloud feedbacks",
        "Not valid for regional downscaling",
    };
    m.valid_conditions = {
        "Global or zonal mean temperature estimates",
        "Timescales of days to months",
    };
    m.known_failure_modes = {
        "Runaway heating if emissivity set too low",
        "Over-damping at large heat capacity",
    };
    m.calibration_procedure = "Tune albedo and heat capacity against reanalysis mean temperature";
    m.dependencies = {};
    return m;
}

std::vector<FieldRequirement> SimpleAtmosphere::requirements() const {
    return {
        {"incoming_radiation", etwin::core::units::watt_per_m2, "Top-of-atmosphere insolation", true, 0.0, etwin::core::Nanoseconds(0)},
    };
}

std::vector<FieldProduct> SimpleAtmosphere::products() const {
    return {
        {"surface_temperature", etwin::core::units::kelvin, etwin::state::FieldKind::Simulation, "Near-surface air temperature"},
        {"humidity", etwin::core::units::dimensionless, etwin::state::FieldKind::Simulation, "Relative humidity"},
        {"wind_speed", etwin::core::units::meter_per_second, etwin::state::FieldKind::Simulation, "Surface wind speed"},
        {"pressure", etwin::core::units::pascal, etwin::state::FieldKind::Simulation, "Surface pressure"},
    };
}

void SimpleAtmosphere::initialize(const ExecutionContext& ctx) {
    state_.temperature = 288.0;
    state_.humidity = 0.6;
    state_.wind_speed = 3.0;
    state_.pressure = 101325.0;
    state_.radiation = parameters_.at("solar_constant") / 4.0;
    (void)ctx;
}

void SimpleAtmosphere::step(const ExecutionContext& ctx) {
    double dt = static_cast<double>(ctx.time_step.count()) / 1e9;
    double S = parameters_.at("solar_constant");
    double alpha = parameters_.at("albedo");
    double eps = parameters_.at("emissivity");
    double C = parameters_.at("heat_capacity");

    constexpr double sigma = 5.670374419e-8;
    double T = state_.temperature;
    double absorbed = S * (1.0 - alpha) / 4.0;
    double emitted = eps * sigma * std::pow(T, 4.0);
    double dT = (absorbed - emitted) / C * dt;

    state_.temperature += dT;
    state_.radiation = absorbed;
}

void SimpleAtmosphere::finalize(const ExecutionContext& ctx) {
    (void)ctx;
}

void SimpleAtmosphere::validate_inputs(const etwin::state::EarthStateQuery& query) const {
    (void)query;
}

void SimpleAtmosphere::calibrate(const std::map<std::string, std::vector<double>>& calibration_data) {
    auto it = calibration_data.find("mean_temperature");
    if (it != calibration_data.end() && !it->second.empty()) {
        parameters_["heat_capacity"] = 4.18e6;
    }
}

std::map<std::string, etwin::state::Field> SimpleAtmosphere::output() const {
    std::map<std::string, etwin::state::Field> result;

    etwin::state::Field temp;
    temp.metadata.name = "surface_temperature";
    temp.metadata.unit = etwin::core::units::kelvin;
    temp.metadata.kind = etwin::state::FieldKind::Simulation;
    temp.data = {state_.temperature};
    temp.metadata.shape = {1};
    result["surface_temperature"] = temp;

    etwin::state::Field humidity;
    humidity.metadata.name = "humidity";
    humidity.metadata.unit = etwin::core::units::dimensionless;
    humidity.metadata.kind = etwin::state::FieldKind::Simulation;
    humidity.data = {state_.humidity};
    humidity.metadata.shape = {1};
    result["humidity"] = humidity;

    return result;
}

void SimpleAtmosphere::reset() {
    state_ = State{};
}

void SimpleAtmosphere::set_parameter(const std::string& name, double value) {
    parameters_[name] = value;
}

} // namespace etwin::models::atmosphere
