#include "models/population/population_model.hpp"

#include <cmath>

namespace etwin::models::population {

ModelMetadata PopulationModel::metadata() const {
    ModelMetadata m;
    m.name = "population_model";
    m.version = "1.0";
    m.description = "Cohort-component population projection at aggregate scales";
    m.equations_or_methods = {
        "Exponential growth with birth, death, migration",
        "Administrative-unit aggregation only",
    };
    m.limitations = {
        "No age structure in base mode",
        "Aggregated (no individual-level data)",
    };
    m.valid_conditions = {
        "Administrative units of 1000+ persons",
        "Decadal projections",
    };
    m.known_failure_modes = {
        "Over-projection without density feedback",
    };
    m.calibration_procedure = "Calibrate rates against census data";
    m.dependencies = {"infrastructure", "economy"};
    return m;
}

std::vector<FieldRequirement> PopulationModel::requirements() const {
    return {
        {"initial_population", etwin::core::units::dimensionless, "Initial population count", true, 0.0, etwin::core::Nanoseconds(0)},
    };
}

std::vector<FieldProduct> PopulationModel::products() const {
    return {
        {"population", etwin::core::units::dimensionless, etwin::state::FieldKind::Simulation, "Aggregated population count"},
        {"population_density", etwin::core::units::dimensionless, etwin::state::FieldKind::Simulation, "Population density"},
    };
}

void PopulationModel::initialize(const ExecutionContext& ctx) {
    state_.total_population = ctx.parameters.contains("initial_population")
        ? ctx.parameters.at("initial_population")
        : 1000000.0;
    state_.births = 0.0;
    state_.deaths = 0.0;
    state_.migration = 0.0;
}

void PopulationModel::step(const ExecutionContext& ctx) {
    double dt = static_cast<double>(ctx.time_step.count()) / 86400e9;
    double years = dt / 365.0;

    double birth_rate = parameters_.at("birth_rate");
    double death_rate = parameters_.at("death_rate");
    double migration_rate = parameters_.at("net_migration_rate");

    state_.births = birth_rate * state_.total_population * years;
    state_.deaths = death_rate * state_.total_population * years;
    state_.migration = migration_rate * state_.total_population * years;
    state_.total_population += state_.births - state_.deaths + state_.migration;

    if (state_.total_population < 0.0) state_.total_population = 0.0;
}

void PopulationModel::finalize(const ExecutionContext& ctx) {
    (void)ctx;
}

void PopulationModel::validate_inputs(const etwin::state::EarthStateQuery& query) const {
    (void)query;
}

void PopulationModel::calibrate(const std::map<std::string, std::vector<double>>& calibration_data) {
    (void)calibration_data;
}

std::map<std::string, etwin::state::Field> PopulationModel::output() const {
    std::map<std::string, etwin::state::Field> result;

    etwin::state::Field pop;
    pop.metadata.name = "population";
    pop.metadata.unit = etwin::core::units::dimensionless;
    pop.metadata.kind = etwin::state::FieldKind::Simulation;
    pop.data = {state_.total_population};
    pop.metadata.shape = {1};
    result["population"] = pop;

    return result;
}

void PopulationModel::reset() {
    state_ = State{};
}

void PopulationModel::set_parameter(const std::string& name, double value) {
    parameters_[name] = value;
}

} // namespace etwin::models::population
