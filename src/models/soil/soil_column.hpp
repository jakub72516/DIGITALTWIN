#pragma once

#include "models/model_module.hpp"

namespace etwin::models::soil {

class SoilColumn : public ModelModule {
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
    std::vector<double> soil_moisture_;
    std::vector<double> soil_temperature_;
    std::map<std::string, double> parameters_{
        {"n_layers", 5.0},
        {"porosity", 0.4},
        {"hydraulic_conductivity", 1e-5},
        {"thermal_conductivity", 1.5},
        {"heat_capacity", 2.0e6},
    };
};

} // namespace etwin::models::soil
