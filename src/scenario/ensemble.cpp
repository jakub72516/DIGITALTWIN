#include "scenario/ensemble.hpp"

#include <algorithm>
#include <cmath>

namespace etwin::scenario {

EnsembleRunner::EnsembleRunner() : rng_(std::random_device{}()) {}

std::vector<double> EnsembleRunner::latin_hypercube_sample(
    size_t n, size_t dims, std::mt19937_64& rng) {
    std::vector<double> result(n * dims);
    for (size_t d = 0; d < dims; ++d) {
        std::vector<double> strata(n);
        for (size_t i = 0; i < n; ++i) strata[i] = static_cast<double>(i);
        std::shuffle(strata.begin(), strata.end(), rng);
        std::uniform_real_distribution<double> u(0.0, 1.0);
        for (size_t i = 0; i < n; ++i) {
            result[i * dims + d] = (strata[i] + u(rng)) / static_cast<double>(n);
        }
    }
    return result;
}

std::map<std::string, ScenarioResult> EnsembleRunner::run_latin_hypercube(
    const ScenarioConfig& config,
    const std::vector<std::pair<std::string, etwin::core::uncertainty::Distribution>>& parameters,
    size_t n_samples) {
    std::map<std::string, ScenarioResult> results;
    auto samples = latin_hypercube_sample(n_samples, parameters.size(), rng_);

    for (size_t i = 0; i < n_samples; ++i) {
        ScenarioConfig member = config;
        member.scenario_id = config.scenario_id + "_lhs_" + std::to_string(i);
        for (size_t p = 0; p < parameters.size(); ++p) {
            double u = samples[i * parameters.size() + p];
            double value = parameters[p].second.param1 +
                u * (parameters[p].second.param2 - parameters[p].second.param1);
            for (auto& mc : member.models) {
                mc.parameters[parameters[p].first] = value;
            }
        }
        ScenarioEngine engine;
        for (const auto& mc : config.models) {
            engine.register_model(mc.model_name, []() {
                return std::unique_ptr<etwin::models::ModelModule>(nullptr);
            });
        }
        results[member.scenario_id] = engine.run(member);
    }
    return results;
}

std::map<std::string, ScenarioResult> EnsembleRunner::run_sobol(
    const ScenarioConfig& config,
    const std::vector<std::pair<std::string, etwin::core::uncertainty::Distribution>>& parameters,
    size_t n_samples) {
    return run_latin_hypercube(config, parameters, n_samples);
}

} // namespace etwin::scenario
