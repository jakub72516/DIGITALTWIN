#pragma once

#include <string>
#include <vector>
#include <map>
#include <random>

#include "core/uncertainty.hpp"
#include "scenario/scenario_engine.hpp"

namespace etwin::scenario {

class EnsembleRunner {
public:
    EnsembleRunner();

    std::map<std::string, ScenarioResult> run_latin_hypercube(
        const ScenarioConfig& config,
        const std::vector<std::pair<std::string, etwin::core::uncertainty::Distribution>>& parameters,
        size_t n_samples);

    std::map<std::string, ScenarioResult> run_sobol(
        const ScenarioConfig& config,
        const std::vector<std::pair<std::string, etwin::core::uncertainty::Distribution>>& parameters,
        size_t n_samples);

    std::vector<double> latin_hypercube_sample(size_t n, size_t dims, std::mt19937_64& rng);

private:
    std::mt19937_64 rng_;
};

} // namespace etwin::scenario
