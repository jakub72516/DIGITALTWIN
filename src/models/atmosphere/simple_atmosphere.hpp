#pragma once

#include "models/model_module.hpp"

namespace etwin::models::atmosphere {

class SimpleAtmosphere : public ModelModule {
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
        double temperature;
        double humidity;
        double wind_speed;
        double pressure;
        double radiation;
    };

    State state_{};
    std::map<std::string, double> parameters_{
        {"solar_constant", 1361.0},
        {"albedo", 0.30},
        {"emissivity", 0.95},
        {"heat_capacity", 4.18e6},
    };
};

} // namespace etwin::models::atmosphere
