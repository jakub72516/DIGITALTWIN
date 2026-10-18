#pragma once

#include "models/model_module.hpp"

namespace etwin::models::ocean {

class LayeredOcean : public ModelModule {
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
    struct Layer {
        double temperature;
        double salinity;
        double thickness;
    };

    std::vector<Layer> layers_;
    std::map<std::string, double> parameters_{
        {"n_layers", 10.0},
        {"mixed_layer_depth", 50.0},
        {"diffusivity", 1e-4},
        {"wind_stress_coefficient", 1.2e-3},
    };
};

} // namespace etwin::models::ocean
