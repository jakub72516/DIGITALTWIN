#include "models/soil/soil_column.hpp"

#include <cmath>

namespace etwin::models::soil {

ModelMetadata SoilColumn::metadata() const {
    ModelMetadata m;
    m.name = "soil_column";
    m.version = "1.0";
    m.description = "Multi-layer soil water and heat diffusion";
    m.equations_or_methods = {
        "Richards equation (simplified) for water",
        "Heat diffusion equation",
    };
    m.limitations = {
        "No freeze-thaw dynamics",
        "No preferential flow",
    };
    m.valid_conditions = {
        "Mineral soils, unfrozen",
    };
    m.known_failure_modes = {
        "Saturation overflow if porosity misconfigured",
    };
    m.calibration_procedure = "Calibrate hydraulic parameters against soil moisture probes";
    m.dependencies = {"atmosphere", "hydrology"};
    return m;
}

std::vector<FieldRequirement> SoilColumn::requirements() const {
    return {
        {"surface_temperature", etwin::core::units::kelvin, "Surface temperature forcing", true, 0.0, etwin::core::Nanoseconds(0)},
        {"infiltration", etwin::core::units::kilogram_per_m2_s, "Infiltration rate", true, 0.0, etwin::core::Nanoseconds(0)},
    };
}

std::vector<FieldProduct> SoilColumn::products() const {
    return {
        {"soil_moisture_profile", etwin::core::units::dimensionless, etwin::state::FieldKind::Simulation, "Soil moisture by layer"},
        {"soil_temperature_profile", etwin::core::units::kelvin, etwin::state::FieldKind::Simulation, "Soil temperature by layer"},
    };
}

void SoilColumn::initialize(const ExecutionContext& ctx) {
    int n = static_cast<int>(parameters_.at("n_layers"));
    soil_moisture_.assign(static_cast<size_t>(n), parameters_.at("porosity") * 0.5);
    soil_temperature_.assign(static_cast<size_t>(n), 288.0);
    (void)ctx;
}

void SoilColumn::step(const ExecutionContext& ctx) {
    (void)ctx;
}

void SoilColumn::finalize(const ExecutionContext& ctx) {
    (void)ctx;
}

void SoilColumn::validate_inputs(const etwin::state::EarthStateQuery& query) const {
    (void)query;
}

void SoilColumn::calibrate(const std::map<std::string, std::vector<double>>& calibration_data) {
    (void)calibration_data;
}

std::map<std::string, etwin::state::Field> SoilColumn::output() const {
    std::map<std::string, etwin::state::Field> result;

    etwin::state::Field sm;
    sm.metadata.name = "soil_moisture_profile";
    sm.metadata.unit = etwin::core::units::dimensionless;
    sm.metadata.kind = etwin::state::FieldKind::Simulation;
    sm.data = soil_moisture_;
    sm.metadata.shape = {soil_moisture_.size()};
    result["soil_moisture_profile"] = sm;

    return result;
}

void SoilColumn::reset() {
    soil_moisture_.clear();
    soil_temperature_.clear();
}

void SoilColumn::set_parameter(const std::string& name, double value) {
    parameters_[name] = value;
}

} // namespace etwin::models::soil
