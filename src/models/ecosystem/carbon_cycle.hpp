#pragma once

#include "models/model_module.hpp"

namespace etwin::models::ecosystem {

class CarbonCycle : public ModelModule {
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
        double vegetation_carbon;
        double soil_carbon;
        double litter_carbon;
        double npp;
        double heterotrophic_respiration;
    };

    State state_{};
    std::map<std::string, double> parameters_{
        {"npp_base", 500.0},
        {"turnover_rate", 0.1},
        {"decomposition_rate", 0.05},
        {"q10", 2.0},
    };
};

} // namespace etwin::models::ecosystem
