#pragma once

#include "models/model_module.hpp"

namespace etwin::models::forestry {

class ForestGap : public ModelModule {
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
        double timber_volume;
        double carbon_stock;
        double stand_age;
        double basal_area;
    };

    State state_{};
    std::map<std::string, double> parameters_{
        {"growth_rate", 0.02},
        {"max_volume", 600.0},
        {"carbon_fraction", 0.5},
        {"natural_mortality", 0.005},
    };
};

} // namespace etwin::models::forestry
