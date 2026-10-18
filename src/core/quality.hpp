#pragma once

#include <string>
#include <vector>
#include <optional>
#include <format>
#include <map>
#include <functional>

#include "provenance.hpp"

namespace etwin::core::quality {

enum class Severity {
    Info,
    Warning,
    Error,
    Critical,
};

struct Issue {
    std::string code;
    std::string description;
    Severity severity;
    std::optional<std::string> field;
    std::optional<double> observed_value;
    std::optional<std::string> expected_range;
};

class ValidationReport {
public:
    void add_issue(const Issue& issue);
    void add_issue(const std::string& code, const std::string& desc, Severity sev);

    bool has_errors() const;
    bool has_critical() const;
    bool is_usable() const;

    const std::vector<Issue>& issues() const { return issues_; }
    const std::map<Severity, size_t>& counts() const { return counts_; }

    std::string summary() const;
    std::string to_json() const;

    void merge(const ValidationReport& other);

private:
    std::vector<Issue> issues_;
    std::map<Severity, size_t> counts_;
};

struct RangeCheck {
    double min;
    double max;
    std::string unit;
};

struct PlausibilityCheck {
    std::string parameter;
    RangeCheck range;
    std::string description;
};

class InputValidator {
public:
    void add_range_check(const std::string& param, double min, double max, const std::string& unit);
    void add_required(const std::string& param);
    void add_custom(const std::string& param, std::function<bool(double)> check);

    ValidationReport validate(const std::map<std::string, double>& values) const;
    ValidationReport validate_schema(const std::map<std::string, std::string>& fields) const;

private:
    std::vector<PlausibilityCheck> range_checks_;
    std::vector<std::string> required_fields_;
    std::map<std::string, std::function<bool(double)>> custom_checks_;
};

struct DataQualityMetrics {
    double completeness = 1.0;
    double accuracy = 1.0;
    double timeliness = 1.0;
    double consistency = 1.0;
    double validity = 1.0;

    double overall() const;
    std::string to_string() const;
};

} // namespace etwin::core::quality
