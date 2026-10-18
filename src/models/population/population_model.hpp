#pragma once

#include "models/model_module.hpp"

namespace etwin::models::population {

class PopulationModel : public ModelModule {
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
        double total_population;
        double births;
        double deaths;
        double migration;
    };

    State state_{};
    std::map<std::string, double> parameters_{
        {"birth_rate", 0.018},
        {"death_rate", 0.008},
        {"net_migration_rate", 0.0},
        {"minimum_aggregation", 1000.0},
    };
};

} // namespace etwin::models::population
