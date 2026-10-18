#include "validation/uncertainty_propagation.hpp"

#include <algorithm>
#include <numeric>
#include <cmath>

namespace etwin::validation {

UncertaintyPropagator::UncertaintyPropagator(uint64_t seed) : rng_(seed) {}

std::vector<double> UncertaintyPropagator::monte_carlo(
    const std::function<double(const std::vector<double>&)>& model_fn,
    const std::vector<etwin::core::uncertainty::Distribution>& input_distributions,
    size_t n_samples) {
    std::vector<double> results;
    results.reserve(n_samples);

    for (size_t s = 0; s < n_samples; ++s) {
        std::vector<double> sample(input_distributions.size());
        for (size_t i = 0; i < input_distributions.size(); ++i) {
            sample[i] = input_distributions[i].sample(rng_);
        }
        results.push_back(model_fn(sample));
    }
    return results;
}

double UncertaintyPropagator::mean(const std::vector<double>& samples) const {
    if (samples.empty()) return 0.0;
    return std::accumulate(samples.begin(), samples.end(), 0.0) / samples.size();
}

double UncertaintyPropagator::std_dev(const std::vector<double>& samples) const {
    if (samples.size() < 2) return 0.0;
    double m = mean(samples);
    double sum = 0.0;
    for (double v : samples) sum += (v - m) * (v - m);
    return std::sqrt(sum / (samples.size() - 1));
}

double UncertaintyPropagator::quantile(const std::vector<double>& samples, double q) const {
    if (samples.empty()) return 0.0;
    std::vector<double> sorted = samples;
    std::sort(sorted.begin(), sorted.end());
    size_t idx = static_cast<size_t>(q * (sorted.size() - 1));
    return sorted[idx];
}

std::map<std::string, double> UncertaintyPropagator::sobol_indices(
    const std::function<double(const std::vector<double>&)>& model_fn,
    const std::vector<etwin::core::uncertainty::Distribution>& input_distributions,
    size_t n_samples) {
    std::map<std::string, double> indices;
    std::vector<double> base = monte_carlo(model_fn, input_distributions, n_samples);
    double base_var = std_dev(base);
    if (base_var <= 0.0) return indices;

    for (size_t i = 0; i < input_distributions.size(); ++i) {
        auto frozen = input_distributions;
        frozen[i] = etwin::core::uncertainty::Distribution::gaussian(
            input_distributions[i].mean(), 0.0);
        std::vector<double> cond = monte_carlo(model_fn, frozen, n_samples);
        double cond_var = std_dev(cond);
        indices["x" + std::to_string(i)] = std::max(0.0, 1.0 - cond_var / base_var);
    }
    return indices;
}

std::vector<SensitivityAnalyzer::SensitivityResult> SensitivityAnalyzer::analyze(
    const std::function<double(const std::map<std::string, double>&)>& model_fn,
    const std::map<std::string, etwin::core::uncertainty::Distribution>& parameters,
    size_t n_samples) {
    std::vector<SensitivityResult> results;
    (void)model_fn;
    (void)parameters;
    (void)n_samples;
    return results;
}

} // namespace etwin::validation
