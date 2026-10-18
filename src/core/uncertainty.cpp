#include "core/uncertainty.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <format>

namespace etwin::core::uncertainty {

Distribution Distribution::gaussian(double mean, double stddev) {
    return Distribution{DistributionType::Gaussian, mean, stddev, std::nullopt};
}

Distribution Distribution::uniform(double lower, double upper) {
    return Distribution{DistributionType::Uniform, lower, upper, std::nullopt};
}

Distribution Distribution::log_normal(double mu, double sigma) {
    return Distribution{DistributionType::LogNormal, mu, sigma, std::nullopt};
}

Distribution Distribution::triangular(double lower, double mode, double upper) {
    return Distribution{DistributionType::Triangular, lower, upper, mode};
}

double Distribution::mean() const {
    switch (type) {
        case DistributionType::Gaussian: return param1;
        case DistributionType::Uniform: return (param1 + param2) / 2.0;
        case DistributionType::LogNormal: return std::exp(param1 + param2 * param2 / 2.0);
        case DistributionType::Triangular: return (param1 + param2 + param3.value_or(param1)) / 3.0;
        default: return param1;
    }
}

double Distribution::stddev() const {
    switch (type) {
        case DistributionType::Gaussian: return param2;
        case DistributionType::Uniform: return (param2 - param1) / std::sqrt(12.0);
        case DistributionType::LogNormal: {
            double s2 = param2 * param2;
            return std::sqrt((std::exp(s2) - 1.0) * std::exp(2.0 * param1 + s2));
        }
        case DistributionType::Triangular: {
            double a = param1, b = param2, c = param3.value_or(param1);
            return std::sqrt((a*a + b*b + c*c - a*b - a*c - b*c) / 18.0);
        }
        default: return 0.0;
    }
}

double Distribution::sample(std::mt19937_64& rng) const {
    switch (type) {
        case DistributionType::Gaussian: {
            std::normal_distribution<double> dist(param1, param2);
            return dist(rng);
        }
        case DistributionType::Uniform: {
            std::uniform_real_distribution<double> dist(param1, param2);
            return dist(rng);
        }
        case DistributionType::LogNormal: {
            std::lognormal_distribution<double> dist(param1, param2);
            return dist(rng);
        }
        case DistributionType::Triangular: {
            std::uniform_real_distribution<double> u(0.0, 1.0);
            double x = u(rng);
            double a = param1, b = param2, c = param3.value_or((a + b) / 2.0);
            double fc = (c - a) / (b - a);
            if (x < fc) return a + std::sqrt(x * (b - a) * (c - a));
            return b - std::sqrt((1.0 - x) * (b - a) * (b - c));
        }
        default: return param1;
    }
}

std::string Distribution::to_string() const {
    switch (type) {
        case DistributionType::Gaussian:
            return "N(" + std::to_string(param1) + "," + std::to_string(param2) + ")";
        case DistributionType::Uniform:
            return "U(" + std::to_string(param1) + "," + std::to_string(param2) + ")";
        case DistributionType::LogNormal:
            return "LogN(" + std::to_string(param1) + "," + std::to_string(param2) + ")";
        case DistributionType::Triangular:
            return "Tri(" + std::to_string(param1) + "," + std::to_string(param2) + "," +
                   std::to_string(param3.value_or(0.0)) + ")";
        default: return "Unknown";
    }
}

UncertainValue::UncertainValue(double nominal, const Distribution& dist)
    : nominal_(nominal), distribution_(dist) {}

double UncertainValue::sample(std::mt19937_64& rng) const {
    return distribution_.sample(rng);
}

double UncertainValue::confidence_lower(double confidence) const {
    double z = 1.96;
    if (confidence >= 0.99) z = 2.576;
    else if (confidence >= 0.95) z = 1.96;
    else if (confidence >= 0.90) z = 1.645;
    return nominal_ - z * distribution_.stddev();
}

double UncertainValue::confidence_upper(double confidence) const {
    double z = 1.96;
    if (confidence >= 0.99) z = 2.576;
    else if (confidence >= 0.95) z = 1.96;
    else if (confidence >= 0.90) z = 1.645;
    return nominal_ + z * distribution_.stddev();
}

UncertainValue UncertainValue::operator+(const UncertainValue& other) const {
    double new_nominal = nominal_ + other.nominal_;
    double new_std = std::sqrt(distribution_.stddev() * distribution_.stddev() +
                              other.distribution_.stddev() * other.distribution_.stddev());
    return UncertainValue(new_nominal, Distribution::gaussian(new_nominal, new_std));
}

UncertainValue UncertainValue::operator-(const UncertainValue& other) const {
    double new_nominal = nominal_ - other.nominal_;
    double new_std = std::sqrt(distribution_.stddev() * distribution_.stddev() +
                              other.distribution_.stddev() * other.distribution_.stddev());
    return UncertainValue(new_nominal, Distribution::gaussian(new_nominal, new_std));
}

UncertainValue UncertainValue::operator*(const UncertainValue& other) const {
    double new_nominal = nominal_ * other.nominal_;
    double rel1 = distribution_.stddev() / std::abs(nominal_);
    double rel2 = other.distribution_.stddev() / std::abs(other.nominal_);
    double new_std = std::abs(new_nominal) * std::sqrt(rel1 * rel1 + rel2 * rel2);
    return UncertainValue(new_nominal, Distribution::gaussian(new_nominal, new_std));
}

UncertainValue UncertainValue::operator/(const UncertainValue& other) const {
    double new_nominal = nominal_ / other.nominal_;
    double rel1 = distribution_.stddev() / std::abs(nominal_);
    double rel2 = other.distribution_.stddev() / std::abs(other.nominal_);
    double new_std = std::abs(new_nominal) * std::sqrt(rel1 * rel1 + rel2 * rel2);
    return UncertainValue(new_nominal, Distribution::gaussian(new_nominal, new_std));
}

std::string UncertainValue::to_string() const {
    return std::format("{} +/- {}", nominal_, distribution_.stddev());
}

Ensemble::Ensemble(size_t n_members) : members_(n_members) {}

void Ensemble::set_member(size_t i, double nominal, const Distribution& dist) {
    if (i >= members_.size()) throw std::out_of_range("Ensemble member index out of range");
    members_[i] = Member{nominal, dist, {}};
}

void Ensemble::add_sample(size_t member, double value) {
    if (member >= members_.size()) throw std::out_of_range("Member index out of range");
    members_[member].samples.push_back(value);
}

double Ensemble::mean() const {
    if (members_.empty()) return 0.0;
    double sum = 0.0;
    for (const auto& m : members_) sum += m.nominal;
    return sum / members_.size();
}

double Ensemble::std_dev() const {
    if (members_.size() < 2) return 0.0;
    double m = mean();
    double sum_sq = 0.0;
    for (const auto& mem : members_) sum_sq += (mem.nominal - m) * (mem.nominal - m);
    return std::sqrt(sum_sq / (members_.size() - 1));
}

double Ensemble::quantile(double q) const {
    std::vector<double> vals;
    for (const auto& m : members_) vals.push_back(m.nominal);
    std::sort(vals.begin(), vals.end());
    if (vals.empty()) return 0.0;
    size_t idx = static_cast<size_t>(q * (vals.size() - 1));
    return vals[idx];
}

std::vector<double> Ensemble::all_samples() const {
    std::vector<double> all;
    for (const auto& m : members_) all.insert(all.end(), m.samples.begin(), m.samples.end());
    return all;
}

void UncertaintyBudget::add_component(const std::string& name, double variance) {
    variances_[name] = variance;
}

void UncertaintyBudget::add_correlated(const std::string& name1, const std::string& name2, double covariance) {
    covariances_.emplace_back(name1, name2, covariance);
}

double UncertaintyBudget::total_variance() const {
    double total = 0.0;
    for (const auto& [name, var] : variances_) total += var;
    for (const auto& [n1, n2, cov] : covariances_) total += 2.0 * cov;
    return total;
}

double UncertaintyBudget::stddev() const {
    return std::sqrt(total_variance());
}

std::map<std::string, double> UncertaintyBudget::contributions() const {
    std::map<std::string, double> result;
    double total = total_variance();
    if (total <= 0.0) return result;
    for (const auto& [name, var] : variances_) {
        result[name] = var / total;
    }
    return result;
}

} // namespace etwin::core::uncertainty
