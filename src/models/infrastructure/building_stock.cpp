#include "models/infrastructure/building_stock.hpp"

#include <cmath>

namespace etwin::models::infrastructure {

ModelMetadata BuildingStock::metadata() const {
    ModelMetadata m;
    m.name = "building_stock";
    m.version = "1.0";
    m.description = "Typology-based building stock model (aggregated, no individual buildings)";
    m.equations_or_methods = {
        "Building stock turnover (construction - demolition)",
        "Energy and water demand from floor area",
    };
    m.limitations = {
        "Aggregated by zone (no individual building data)",
        "No renovation dynamics",
    };
    m.valid_conditions = {
        "Urban and peri-urban zones",
    };
    m.known_failure_modes = {
        "Energy demand inaccurate without occupancy data",
    };
    m.calibration_procedure = "Calibrate energy intensity against utility data";
    m.dependencies = {"population", "energy"};
    return m;
}

std::vector<FieldRequirement> BuildingStock::requirements() const {
    return {
        {"population", etwin::core::units::dimensionless, "Zone population", true, 0.0, etwin::core::Nanoseconds(0)},
    };
}

std::vector<FieldProduct> BuildingStock::products() const {
    return {
        {"building_count", etwin::core::units::dimensionless, etwin::state::FieldKind::Simulation, "Building count by zone"},
        {"floor_area", etwin::core::units::dimensionless, etwin::state::FieldKind::Simulation, "Total floor area"},
        {"building_energy_demand", etwin::core::units::watt_per_m2, etwin::state::FieldKind::Simulation, "Building energy demand"},
        {"building_water_demand", etwin::core::units::dimensionless, etwin::state::FieldKind::Simulation, "Building water demand"},
    };
}

void BuildingStock::initialize(const ExecutionContext& ctx) {
    state_.building_count = ctx.parameters.contains("population")
        ? ctx.parameters.at("population") * 0.4
        : 100000.0;
    state_.floor_area = state_.building_count * 150.0;
    state_.energy_demand = 0.0;
    state_.water_demand = 0.0;
}

void BuildingStock::step(const ExecutionContext& ctx) {
    double dt = static_cast<double>(ctx.time_step.count()) / 86400e9;

    double construction = parameters_.at("construction_rate");
    double demolition = parameters_.at("demolition_rate");

    state_.building_count += (construction - demolition) * state_.building_count * dt;
    state_.floor_area = state_.building_count * 150.0;
    state_.energy_demand = state_.floor_area * parameters_.at("energy_per_area");
    state_.water_demand = ctx.parameters.contains("population")
        ? ctx.parameters.at("population") * parameters_.at("water_per_person")
        : 0.0;
}

void BuildingStock::finalize(const ExecutionContext& ctx) {
    (void)ctx;
}

void BuildingStock::validate_inputs(const etwin::state::EarthStateQuery& query) const {
    (void)query;
}

void BuildingStock::calibrate(const std::map<std::string, std::vector<double>>& calibration_data) {
    (void)calibration_data;
}

std::map<std::string, etwin::state::Field> BuildingStock::output() const {
    std::map<std::string, etwin::state::Field> result;

    etwin::state::Field bc;
    bc.metadata.name = "building_count";
    bc.metadata.unit = etwin::core::units::dimensionless;
    bc.metadata.kind = etwin::state::FieldKind::Simulation;
    bc.data = {state_.building_count};
    bc.metadata.shape = {1};
    result["building_count"] = bc;

    etwin::state::Field ed;
    ed.metadata.name = "building_energy_demand";
    ed.metadata.unit = etwin::core::units::watt_per_m2;
    ed.metadata.kind = etwin::state::FieldKind::Simulation;
    ed.data = {state_.energy_demand};
    ed.metadata.shape = {1};
    result["building_energy_demand"] = ed;

    return result;
}

void BuildingStock::reset() {
    state_ = State{};
}

void BuildingStock::set_parameter(const std::string& name, double value) {
    parameters_[name] = value;
}

} // namespace etwin::models::infrastructure
