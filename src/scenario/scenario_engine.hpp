#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <random>
#include <functional>

#include "core/time.hpp"
#include "core/provenance.hpp"

#include "models/model_module.hpp"
#include "state/earth_state.hpp"
#include "coupling/coupling_service.hpp"

namespace etwin::scenario {

enum class ScenarioType {
    Baseline,
    Intervention,
    Ensemble,
    Sensitivity,
};

struct ModelConfig {
    std::string model_name;
    std::map<std::string, double> parameters;
    uint64_t seed;
};

struct Intervention {
    std::string name;
    std::string description;
    std::string target_model;
    std::string parameter;
    double value;
    etwin::core::Instant start_time;
};

struct ScenarioConfig {
    std::string scenario_id;
    std::string name;
    ScenarioType type;
    std::string description;

    etwin::core::Instant start_time;
    etwin::core::Instant end_time;
    etwin::core::Nanoseconds time_step;

    std::vector<ModelConfig> models;
    std::vector<etwin::coupling::CouplingLink> coupling_links;
    std::vector<Intervention> interventions;

    std::string initial_state_hash;
    std::vector<std::string> forcing_datasets;
};

struct ScenarioResult {
    std::string scenario_id;
    bool success;
    std::string status;
    etwin::core::Hash run_hash;
    etwin::state::EarthState final_state;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    std::map<std::string, double> summary_metrics;
};

class ScenarioEngine {
public:
    ScenarioEngine();

    ScenarioResult run(const ScenarioConfig& config);

    ScenarioResult run_baseline(const ScenarioConfig& config);
    ScenarioResult run_intervention(const ScenarioConfig& base, const Intervention& intervention);

    void register_model(const std::string& name,
                        std::function<std::unique_ptr<etwin::models::ModelModule>()> creator);

    std::map<std::string, ScenarioResult> run_ensemble(
        const ScenarioConfig& config, size_t n_members, uint64_t base_seed);

    std::map<std::string, ScenarioResult> run_sensitivity(
        const ScenarioConfig& config, const std::string& parameter,
        const std::vector<double>& values);

private:
    std::map<std::string, std::function<std::unique_ptr<etwin::models::ModelModule>()>> model_registry_;
    etwin::coupling::CouplingService coupling_;
    std::mt19937_64 rng_;
};

} // namespace etwin::scenario
