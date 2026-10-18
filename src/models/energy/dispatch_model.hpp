#pragma once

#include "models/model_module.hpp"

namespace etwin::models::energy {

class DispatchModel : public ModelModule {
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
        double demand;
        double renewable_generation;
        double fossil_generation;
        double storage_charge;
        double emissions;
        double curtailment;
    };

    State state_{};
    std::map<std::string, double> parameters_{
        {"renewable_capacity", 1000.0},
        {"fossil_capacity", 1500.0},
        {"storage_capacity", 200.0},
        {"emission_intensity_fossil", 0.5},
        {"renewable_capacity_factor", 0.3},
    };
};

} // namespace etwin::models::energy
