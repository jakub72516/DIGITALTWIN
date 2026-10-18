#pragma once

#include "models/model_module.hpp"

namespace etwin::models::hydrology {

class HBV : public ModelModule {
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
        double snow_storage;
        double soil_moisture;
        double upper_zone;
        double lower_zone;
    };

    State state_{};
    std::map<std::string, double> parameters_{
        {"fc", 200.0},
        {"beta", 2.0},
        {"pwp", 50.0},
        {"k0", 0.3},
        {"k1", 0.1},
        {"k2", 0.05},
        {"snow_melt_threshold", 0.0},
    };
};

} // namespace etwin::models::hydrology
