#pragma once

#include <string>
#include <vector>
#include <map>

#include "core/time.hpp"
#include "validation/validator.hpp"

namespace etwin::validation {

struct HindcastSeries {
    std::string variable;
    std::vector<double> predicted;
    std::vector<double> observed;
    std::vector<etwin::core::Instant> times;
};

struct HindcastReport {
    std::string model_name;
    etwin::core::TimeRange period;
    bool completed;
    std::map<std::string, ValidationResult> variable_results;
    std::vector<std::string> notes;
};

class HindcastService {
public:
    HindcastReport evaluate(
        const std::string& model_name,
        const etwin::core::TimeRange& period,
        const std::map<std::string, HindcastSeries>& series) const;

    std::map<std::string, double> skill_scores(const HindcastSeries& series) const;

    static double crps(const std::vector<double>& ensemble,
                       double observation);
    static double brier_score(const std::vector<double>& probabilities,
                              const std::vector<int>& outcomes);
    static double reliability(const std::vector<double>& predicted_prob,
                              const std::vector<int>& outcomes);
};

} // namespace etwin::validation
