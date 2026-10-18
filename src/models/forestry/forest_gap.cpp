#include "models/forestry/forest_gap.hpp"

#include <cmath>

namespace etwin::models::forestry {

ModelMetadata ForestGap::metadata() const {
    ModelMetadata m;
    m.name = "forest_gap";
    m.version = "1.0";
    m.description = "Gap-model and 3-PG style forest growth model";
    m.equations_or_methods = {
        "Logistic growth to carrying capacity",
        "Carbon allocation to timber",
        "Natural mortality",
    };
    m.limitations = {
        "Single age-cohort stand",
        "No species competition",
    };
    m.valid_conditions = {
        "Temperate and boreal forests",
        "Decadal growth dynamics",
    };
    m.known_failure_modes = {
        "Overestimates growth without density constraints",
    };
    m.calibration_procedure = "Calibrate growth rate and max volume against forest inventories";
    m.dependencies = {"atmosphere", "soil"};
    return m;
}

std::vector<FieldRequirement> ForestGap::requirements() const {
    return {
        {"temperature", etwin::core::units::kelvin, "Mean temperature", true, 0.0, etwin::core::Nanoseconds(0)},
        {"precipitation", etwin::core::units::kilogram_per_m2_s, "Precipitation", true, 0.0, etwin::core::Nanoseconds(0)},
        {"co2_concentration", etwin::core::units::dimensionless, "Atmospheric CO2", true, 0.0, etwin::core::Nanoseconds(0)},
    };
}

std::vector<FieldProduct> ForestGap::products() const {
    return {
        {"timber_volume", etwin::core::units::dimensionless, etwin::state::FieldKind::Simulation, "Timber volume"},
        {"forest_carbon", etwin::core::units::kilogram_per_m2, etwin::state::FieldKind::Simulation, "Forest carbon stock"},
    };
}

void ForestGap::initialize(const ExecutionContext& ctx) {
    state_.timber_volume = 50.0;
    state_.carbon_stock = 50.0 * parameters_.at("carbon_fraction");
    state_.stand_age = 20.0;
    state_.basal_area = 10.0;
    (void)ctx;
}

void ForestGap::step(const ExecutionContext& ctx) {
    double dt = static_cast<double>(ctx.time_step.count()) / 86400e9;

    double growth = parameters_.at("growth_rate");
    double max_vol = parameters_.at("max_volume");
    double mortality = parameters_.at("natural_mortality");

    double logistic = growth * state_.timber_volume * (1.0 - state_.timber_volume / max_vol);
    state_.timber_volume += (logistic - mortality * state_.timber_volume) * dt;
    state_.carbon_stock = state_.timber_volume * parameters_.at("carbon_fraction");
    state_.stand_age += dt / 365.0;
}

void ForestGap::finalize(const ExecutionContext& ctx) {
    (void)ctx;
}

void ForestGap::validate_inputs(const etwin::state::EarthStateQuery& query) const {
    (void)query;
}

void ForestGap::calibrate(const std::map<std::string, std::vector<double>>& calibration_data) {
    (void)calibration_data;
}

std::map<std::string, etwin::state::Field> ForestGap::output() const {
    std::map<std::string, etwin::state::Field> result;

    etwin::state::Field tv;
    tv.metadata.name = "timber_volume";
    tv.metadata.unit = etwin::core::units::dimensionless;
    tv.metadata.kind = etwin::state::FieldKind::Simulation;
    tv.data = {state_.timber_volume};
    tv.metadata.shape = {1};
    result["timber_volume"] = tv;

    etwin::state::Field fc;
    fc.metadata.name = "forest_carbon";
    fc.metadata.unit = etwin::core::units::kilogram_per_m2;
    fc.metadata.kind = etwin::state::FieldKind::Simulation;
    fc.data = {state_.carbon_stock};
    fc.metadata.shape = {1};
    result["forest_carbon"] = fc;

    return result;
}

void ForestGap::reset() {
    state_ = State{};
}

void ForestGap::set_parameter(const std::string& name, double value) {
    parameters_[name] = value;
}

} // namespace etwin::models::forestry
