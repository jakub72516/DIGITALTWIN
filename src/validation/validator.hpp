#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>

#include "core/time.hpp"
#include "state/earth_state.hpp"
#include "models/model_module.hpp"

namespace etwin::validation {

struct ValidationMetric {
    std::string name;
    double value;
    bool higher_is_better;
    std::string unit;
};

struct ValidationResult {
    std::string model_name;
    std::string validation_dataset;
    bool passed;
    std::vector<ValidationMetric> metrics;
    std::vector<std::string> failure_reasons;
    std::map<std::string, double> thresholds;
};

class ModelValidator {
public:
    void add_metric(const std::string& name, double threshold, bool higher_is_better);

    ValidationResult validate_model(
        const std::string& model_name,
        const std::map<std::string, std::vector<double>>& model_output,
        const std::map<std::string, std::vector<double>>& observations) const;

    double rmse(const std::vector<double>& predicted, const std::vector<double>& observed) const;
    double bias(const std::vector<double>& predicted, const std::vector<double>& observed) const;
    double correlation(const std::vector<double>& predicted, const std::vector<double>& observed) const;
    double nash_sutcliffe(const std::vector<double>& predicted, const std::vector<double>& observed) const;
    double mean_absolute_error(const std::vector<double>& predicted, const std::vector<double>& observed) const;
    double kling_gupta(const std::vector<double>& predicted, const std::vector<double>& observed) const;

private:
    std::map<std::string, std::pair<double, bool>> metric_thresholds_;
};

struct HindcastConfig {
    etwin::core::TimeRange period;
    std::string validation_dataset;
    std::vector<std::string> variables;
};

struct HindcastResult {
    bool success;
    std::map<std::string, ValidationResult> per_variable;
    std::vector<std::string> warnings;
};

class HindcastRunner {
public:
    HindcastResult run(
        const std::string& model_name,
        const etwin::models::ModelModule& model,
        const HindcastConfig& config,
        const std::map<std::string, std::vector<double>>& observations) const;
};

} // namespace etwin::validation
