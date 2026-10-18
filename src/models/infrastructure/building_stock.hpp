#pragma once

#include "models/model_module.hpp"

namespace etwin::models::infrastructure {

class BuildingStock : public ModelModule {
public:
    ModelMetadata metadata() const override;
    std::vector<FieldRequirement> requirements() const override;
    std::vector<FieldProduct> products() const override;

    void initialize(const ExecutionContext& ctx) override;
    void step(const ExecutionContext& ctx) override;
    void finalize(const ExecutionContext& ctx) override;

    void validate_inputs(const etwin::state::EarthStateQuery& query) const override;
    void calibrate(const std::map<std::string, std::vector<double>>& calibration_data) override;

    std::map<std::string, etwin::state::Field> output() const override;
    void reset() override;

    void set_parameter(const std::string& name, double value);

private:
    struct State {
        double building_count;
        double floor_area;
        double energy_demand;
        double water_demand;
    };

    State state_{};
    std::map<std::string, double> parameters_{
        {"construction_rate", 0.01},
        {"demolition_rate", 0.002},
        {"energy_per_area", 150.0},
        {"water_per_person", 0.15},
    };
};

} // namespace etwin::models::infrastructure
