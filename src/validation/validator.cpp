#include "validation/validator.hpp"

#include <cmath>
#include <numeric>
#include <algorithm>

namespace etwin::validation {

void ModelValidator::add_metric(const std::string& name, double threshold, bool higher_is_better) {
    metric_thresholds_[name] = {threshold, higher_is_better};
}

double ModelValidator::rmse(const std::vector<double>& predicted, const std::vector<double>& observed) const {
    if (predicted.size() != observed.size() || predicted.empty()) return std::numeric_limits<double>::quiet_NaN();
    double sum = 0.0;
    for (size_t i = 0; i < predicted.size(); ++i) {
        double d = predicted[i] - observed[i];
        sum += d * d;
    }
    return std::sqrt(sum / predicted.size());
}

double ModelValidator::bias(const std::vector<double>& predicted, const std::vector<double>& observed) const {
    if (predicted.size() != observed.size() || predicted.empty()) return std::numeric_limits<double>::quiet_NaN();
    double sum = 0.0;
    for (size_t i = 0; i < predicted.size(); ++i) {
        sum += predicted[i] - observed[i];
    }
    return sum / predicted.size();
}

double ModelValidator::mean_absolute_error(const std::vector<double>& predicted, const std::vector<double>& observed) const {
    if (predicted.size() != observed.size() || predicted.empty()) return std::numeric_limits<double>::quiet_NaN();
    double sum = 0.0;
    for (size_t i = 0; i < predicted.size(); ++i) {
        sum += std::abs(predicted[i] - observed[i]);
    }
    return sum / predicted.size();
}

double ModelValidator::correlation(const std::vector<double>& predicted, const std::vector<double>& observed) const {
    if (predicted.size() != observed.size() || predicted.size() < 2) return std::numeric_limits<double>::quiet_NaN();
    double mean_p = std::accumulate(predicted.begin(), predicted.end(), 0.0) / predicted.size();
    double mean_o = std::accumulate(observed.begin(), observed.end(), 0.0) / observed.size();
    double num = 0.0, den_p = 0.0, den_o = 0.0;
    for (size_t i = 0; i < predicted.size(); ++i) {
        num += (predicted[i] - mean_p) * (observed[i] - mean_o);
        den_p += (predicted[i] - mean_p) * (predicted[i] - mean_p);
        den_o += (observed[i] - mean_o) * (observed[i] - mean_o);
    }
    if (den_p == 0.0 || den_o == 0.0) return 0.0;
    return num / std::sqrt(den_p * den_o);
}

double ModelValidator::nash_sutcliffe(const std::vector<double>& predicted, const std::vector<double>& observed) const {
    if (predicted.size() != observed.size() || predicted.empty()) return std::numeric_limits<double>::quiet_NaN();
    double mean_o = std::accumulate(observed.begin(), observed.end(), 0.0) / observed.size();
    double num = 0.0, den = 0.0;
    for (size_t i = 0; i < predicted.size(); ++i) {
        num += (predicted[i] - observed[i]) * (predicted[i] - observed[i]);
        den += (observed[i] - mean_o) * (observed[i] - mean_o);
    }
    if (den == 0.0) return 0.0;
    return 1.0 - num / den;
}

double ModelValidator::kling_gupta(const std::vector<double>& predicted, const std::vector<double>& observed) const {
    double r = correlation(predicted, observed);
    if (std::isnan(r)) return std::numeric_limits<double>::quiet_NaN();
    double mean_p = std::accumulate(predicted.begin(), predicted.end(), 0.0) / predicted.size();
    double mean_o = std::accumulate(observed.begin(), observed.end(), 0.0) / observed.size();
    double std_p = 0.0, std_o = 0.0;
    for (size_t i = 0; i < predicted.size(); ++i) {
        std_p += (predicted[i] - mean_p) * (predicted[i] - mean_p);
        std_o += (observed[i] - mean_o) * (observed[i] - mean_o);
    }
    std_p = std::sqrt(std_p / (predicted.size() - 1));
    std_o = std::sqrt(std_o / (observed.size() - 1));
    double alpha = std_o > 0.0 ? std_p / std_o : 0.0;
    double beta = std_o > 0.0 ? mean_p / std_o : 0.0;
    double gamma = mean_o / std_o;
    double beta_term = beta - gamma;
    return 1.0 - std::sqrt((r - 1.0) * (r - 1.0) + (alpha - 1.0) * (alpha - 1.0) + beta_term * beta_term);
}

ValidationResult ModelValidator::validate_model(
    const std::string& model_name,
    const std::map<std::string, std::vector<double>>& model_output,
    const std::map<std::string, std::vector<double>>& observations) const {
    ValidationResult result;
    result.model_name = model_name;
    result.passed = true;

    for (const auto& [name, threshold_info] : metric_thresholds_) {
        auto po = model_output.find(name);
        auto ob = observations.find(name);
        if (po == model_output.end() || ob == observations.end()) {
            result.failure_reasons.push_back("Missing data for metric: " + name);
            result.passed = false;
            continue;
        }
        double value = rmse(po->second, ob->second);
        result.metrics.push_back({name, value, threshold_info.second, "varies"});
        bool pass = threshold_info.second ? value >= threshold_info.first : value <= threshold_info.first;
        result.thresholds[name] = threshold_info.first;
        if (!pass) {
            result.failure_reasons.push_back("Metric " + name + " = " + std::to_string(value) +
                " does not meet threshold " + std::to_string(threshold_info.first));
            result.passed = false;
        }
    }
    return result;
}

HindcastResult HindcastRunner::run(
    const std::string& model_name,
    const etwin::models::ModelModule& model,
    const HindcastConfig& config,
    const std::map<std::string, std::vector<double>>& observations) const {
    HindcastResult result;
    result.success = true;

    ModelValidator validator;
    validator.add_metric("rmse", 0.0, false);

    auto products = model.products();
    for (const auto& p : products) {
        auto it = observations.find(p.name);
        if (it == observations.end()) {
            result.warnings.push_back("No observations for variable: " + p.name);
            continue;
        }
    }
    return result;
}

} // namespace etwin::validation
