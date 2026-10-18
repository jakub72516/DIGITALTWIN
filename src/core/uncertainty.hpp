#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <random>
#include <memory>
#include <numbers>
#include <cmath>
#include <optional>
#include <map>
#include <tuple>

namespace etwin::core::uncertainty {

enum class DistributionType {
    Gaussian,
    Uniform,
    LogNormal,
    Triangular,
    Empirical,
};

struct Distribution {
    DistributionType type;
    double param1;
    double param2;
    std::optional<double> param3;

    static Distribution gaussian(double mean, double stddev);
    static Distribution uniform(double lower, double upper);
    static Distribution log_normal(double mu, double sigma);
    static Distribution triangular(double lower, double mode, double upper);

    double mean() const;
    double stddev() const;
    double sample(std::mt19937_64& rng) const;

    std::string to_string() const;
};

class UncertainValue {
public:
    UncertainValue() = default;
    UncertainValue(double nominal, const Distribution& dist);

    double nominal() const { return nominal_; }
    const Distribution& distribution() const { return distribution_; }

    double sample(std::mt19937_64& rng) const;
    double confidence_lower(double confidence = 0.95) const;
    double confidence_upper(double confidence = 0.95) const;

    UncertainValue operator+(const UncertainValue& other) const;
    UncertainValue operator-(const UncertainValue& other) const;
    UncertainValue operator*(const UncertainValue& other) const;
    UncertainValue operator/(const UncertainValue& other) const;

    std::string to_string() const;

private:
    double nominal_ = 0.0;
    Distribution distribution_{DistributionType::Gaussian, 0.0, 0.0};
};

class Ensemble {
public:
    Ensemble() = default;
    explicit Ensemble(size_t n_members);

    void set_member(size_t i, double nominal, const Distribution& dist);
    void add_sample(size_t member, double value);

    size_t size() const { return members_.size(); }
    double mean() const;
    double std_dev() const;
    double quantile(double q) const;
    std::vector<double> all_samples() const;

private:
    struct Member {
        double nominal;
        Distribution distribution;
        std::vector<double> samples;
    };
    std::vector<Member> members_;
};

class UncertaintyBudget {
public:
    void add_component(const std::string& name, double variance);
    void add_correlated(const std::string& name1, const std::string& name2, double covariance);

    double total_variance() const;
    double stddev() const;
    std::map<std::string, double> contributions() const;

private:
    std::map<std::string, double> variances_;
    std::vector<std::tuple<std::string, std::string, double>> covariances_;
};

} // namespace etwin::core::uncertainty
