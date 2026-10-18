#pragma once

#include "models/model_module.hpp"

namespace etwin::models::geohazard {

class Coulomb : public ModelModule {
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
        double shear_stress;
        double normal_stress;
        double pore_pressure;
    };

    State state_{};
    std::map<std::string, double> parameters_{
        {"friction_coefficient", 0.6},
        {"cohesion", 20.0},
        {"skempton_coefficient", 0.7},
    };
};

} // namespace etwin::models::geohazard
