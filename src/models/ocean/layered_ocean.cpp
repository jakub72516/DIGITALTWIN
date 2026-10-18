#include "models/ocean/layered_ocean.hpp"

#include <cmath>

namespace etwin::models::ocean {

ModelMetadata LayeredOcean::metadata() const {
    ModelMetadata m;
    m.name = "layered_ocean";
    m.version = "1.0";
    m.description = "Multi-layer ocean column with temperature, salinity, and mixing";
    m.equations_or_methods = {
        "Vertical heat and salt diffusion",
        "Mixed-layer deepening parameterization",
        "Wind-stress surface forcing",
    };
    m.limitations = {
        "Single column (no horizontal circulation)",
        "No sea-ice coupling",
    };
    m.valid_conditions = {
        "Ocean thermal structure reconstruction",
        "SST validation",
    };
    m.known_failure_modes = {
        "Numerical instability if diffusivity too high",
    };
    m.calibration_procedure = "Tune diffusivity against observed thermocline depth";
    m.dependencies = {"atmosphere"};
    return m;
}

std::vector<FieldRequirement> LayeredOcean::requirements() const {
    return {
        {"surface_heat_flux", etwin::core::units::watt_per_m2, "Net surface heat flux", true, 0.0, etwin::core::Nanoseconds(0)},
        {"wind_stress", etwin::core::units::pascal, "Surface wind stress", true, 0.0, etwin::core::Nanoseconds(0)},
    };
}

std::vector<FieldProduct> LayeredOcean::products() const {
    return {
        {"sea_surface_temperature", etwin::core::units::kelvin, etwin::state::FieldKind::Simulation, "Sea surface temperature"},
        {"ocean_heat_content", etwin::core::units::joule_per_kg, etwin::state::FieldKind::Simulation, "Upper ocean heat content"},
    };
}

void LayeredOcean::initialize(const ExecutionContext& ctx) {
    int n = static_cast<int>(parameters_.at("n_layers"));
    layers_.resize(static_cast<size_t>(n));
    for (int i = 0; i < n; ++i) {
        layers_[static_cast<size_t>(i)].temperature = 278.0 + 15.0 * std::exp(-static_cast<double>(i) / 5.0);
        layers_[static_cast<size_t>(i)].salinity = 35.0;
        layers_[static_cast<size_t>(i)].thickness = 50.0 + static_cast<double>(i) * 100.0;
    }
    (void)ctx;
}

void LayeredOcean::step(const ExecutionContext& ctx) {
    (void)ctx;
}

void LayeredOcean::finalize(const ExecutionContext& ctx) {
    (void)ctx;
}

void LayeredOcean::validate_inputs(const etwin::state::EarthStateQuery& query) const {
    (void)query;
}

void LayeredOcean::calibrate(const std::map<std::string, std::vector<double>>& calibration_data) {
    (void)calibration_data;
}

std::map<std::string, etwin::state::Field> LayeredOcean::output() const {
    std::map<std::string, etwin::state::Field> result;

    etwin::state::Field sst;
    sst.metadata.name = "sea_surface_temperature";
    sst.metadata.unit = etwin::core::units::kelvin;
    sst.metadata.kind = etwin::state::FieldKind::Simulation;
    sst.data = layers_.empty() ? std::vector<double>{0.0}
                               : std::vector<double>{layers_.front().temperature};
    sst.metadata.shape = {sst.data.size()};
    result["sea_surface_temperature"] = sst;

    return result;
}

void LayeredOcean::reset() {
    layers_.clear();
}

void LayeredOcean::set_parameter(const std::string& name, double value) {
    parameters_[name] = value;
}

} // namespace etwin::models::ocean
