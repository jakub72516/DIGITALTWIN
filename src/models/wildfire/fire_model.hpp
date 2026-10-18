#pragma once

#include "models/model_module.hpp"

namespace etwin::models::wildfire {

class FireModel : public ModelModule {
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
        double fire_weather_index;
        double burned_area;
        double fuel_moisture;
        double emissions;
    };

    State state_{};
    std::map<std::string, double> parameters_{
        {"spread_rate_coefficient", 0.1},
        {"fuel_loading", 5.0},
        {"emission_factor", 0.45},
        {"ignition_probability", 0.001},
    };
};

} // namespace etwin::models::wildfire
