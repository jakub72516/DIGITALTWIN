#include "validation/hindcast.hpp"

#include <cmath>
#include <numeric>
#include <algorithm>

namespace etwin::validation {

HindcastReport HindcastService::evaluate(
    const std::string& model_name,
    const etwin::core::TimeRange& period,
    const std::map<std::string, HindcastSeries>& series) const {
    HindcastReport report;
    report.model_name = model_name;
    report.period = period;
    report.completed = true;

    ModelValidator validator;
    validator.add_metric("rmse", 0.0, false);

    for (const auto& [name, s] : series) {
        ValidationResult result;
        result.model_name = model_name;
        result.validation_dataset = "hindcast";
        result.passed = true;

        if (s.predicted.empty() || s.observed.empty()) {
            result.passed = false;
            result.failure_reasons.push_back("Empty series for " + name);
        } else {
            result.metrics.push_back({"rmse", validator.rmse(s.predicted, s.observed), false, "varies"});
            result.metrics.push_back({"bias", validator.bias(s.predicted, s.observed), false, "varies"});
            result.metrics.push_back({"nash_sutcliffe", validator.nash_sutcliffe(s.predicted, s.observed), true, "varies"});
        }
        report.variable_results[name] = result;
    }
    return report;
}

std::map<std::string, double> HindcastService::skill_scores(const HindcastSeries& series) const {
    std::map<std::string, double> scores;
    ModelValidator validator;
    scores["rmse"] = validator.rmse(series.predicted, series.observed);
    scores["bias"] = validator.bias(series.predicted, series.observed);
    scores["nash_sutcliffe"] = validator.nash_sutcliffe(series.predicted, series.observed);
    scores["correlation"] = validator.correlation(series.predicted, series.observed);
    return scores;
}

double HindcastService::crps(const std::vector<double>& ensemble, double observation) {
    if (ensemble.empty()) return 0.0;
    std::vector<double> sorted = ensemble;
    std::sort(sorted.begin(), sorted.end());
    size_t n = sorted.size();
    double sum = 0.0;
    for (size_t i = 0; i < n; ++i) {
        double x = sorted[i];
        double empirical = static_cast<double>(i) / static_cast<double>(n);
        double cdf = x <= observation ? 1.0 : 0.0;
        sum += (empirical - cdf) * (empirical - cdf);
    }
    return sum / n;
}

double HindcastService::brier_score(const std::vector<double>& probabilities, const std::vector<int>& outcomes) {
    if (probabilities.size() != outcomes.size() || probabilities.empty()) return 0.0;
    double sum = 0.0;
    for (size_t i = 0; i < probabilities.size(); ++i) {
        sum += (probabilities[i] - outcomes[i]) * (probabilities[i] - outcomes[i]);
    }
    return sum / probabilities.size();
}

double HindcastService::reliability(const std::vector<double>& predicted_prob, const std::vector<int>& outcomes) {
    return 1.0 - brier_score(predicted_prob, outcomes);
}

} // namespace etwin::validation
