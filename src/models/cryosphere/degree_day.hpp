#pragma once

#include "models/model_module.hpp"

namespace etwin::models::cryosphere {

class DegreeDay : public ModelModule {
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
        double snow_water_equivalent;
        double ice_thickness;
        double albedo;
    };

    State state_{};
    std::map<std::string, double> parameters_{
        {"degree_day_factor_snow", 4.0},
        {"degree_day_factor_ice", 8.0},
        {"melt_threshold", 0.0},
        {"albedo_snow", 0.8},
        {"albedo_ice", 0.4},
    };
};

} // namespace etwin::models::cryosphere
