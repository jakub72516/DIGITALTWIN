#pragma once

#include "models/model_module.hpp"

namespace etwin::models::agriculture {

class CropModel : public ModelModule {
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
        double biomass;
        double leaf_area_index;
        double accumulated_gdd;
        double yield;
        bool mature;
    };

    State state_{};
    std::map<std::string, double> parameters_{
        {"base_temperature", 10.0},
        {"gdd_to_maturity", 1500.0},
        {"radiation_use_efficiency", 2.5},
        {"harvest_index", 0.45},
        {"water_stress_threshold", 0.4},
    };
};

} // namespace etwin::models::agriculture
