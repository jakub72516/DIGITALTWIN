#pragma once

#include <string>
#include <vector>
#include <map>
#include <random>
#include <functional>

#include "core/uncertainty.hpp"

namespace etwin::validation {

struct UncertaintyEstimate {
    std::string parameter;
    double mean;
    double std_dev;
    double variance;
    std::vector<double> samples;
};

class UncertaintyPropagator {
public:
    explicit UncertaintyPropagator(uint64_t seed);

    std::vector<double> monte_carlo(
        const std::function<double(const std::vector<double>&)>& model_fn,
        const std::vector<etwin::core::uncertainty::Distribution>& input_distributions,
        size_t n_samples);

    double mean(const std::vector<double>& samples) const;
    double std_dev(const std::vector<double>& samples) const;
    double quantile(const std::vector<double>& samples, double q) const;

    std::map<std::string, double> sobol_indices(
        const std::function<double(const std::vector<double>&)>& model_fn,
        const std::vector<etwin::core::uncertainty::Distribution>& input_distributions,
        size_t n_samples);

    std::mt19937_64& rng() { return rng_; }

private:
    std::mt19937_64 rng_;
};

class SensitivityAnalyzer {
public:
    struct SensitivityResult {
        std::string parameter;
        double first_order_index;
        double total_order_index;
    };

    std::vector<SensitivityResult> analyze(
        const std::function<double(const std::map<std::string, double>&)>& model_fn,
        const std::map<std::string, etwin::core::uncertainty::Distribution>& parameters,
        size_t n_samples);
};

} // namespace etwin::validation
