#include "core/quality.hpp"
#include <sstream>

namespace etwin::core::quality {

void ValidationReport::add_issue(const Issue& issue) {
    issues_.push_back(issue);
    counts_[issue.severity]++;
}

void ValidationReport::add_issue(const std::string& code, const std::string& desc, Severity sev) {
    add_issue(Issue{code, desc, sev, std::nullopt, std::nullopt, std::nullopt});
}

bool ValidationReport::has_errors() const {
    auto it = counts_.find(Severity::Error);
    return it != counts_.end() && it->second > 0;
}

bool ValidationReport::has_critical() const {
    auto it = counts_.find(Severity::Critical);
    return it != counts_.end() && it->second > 0;
}

bool ValidationReport::is_usable() const {
    return !has_critical() && !has_errors();
}

std::string ValidationReport::summary() const {
    std::ostringstream oss;
    oss << "Validation: " << issues_.size() << " issues";
    if (!counts_.empty()) {
        oss << " (";
        bool first = true;
        for (const auto& [sev, cnt] : counts_) {
            if (!first) oss << ", ";
            first = false;
            oss << static_cast<int>(sev) << ":" << cnt;
        }
        oss << ")";
    }
    return oss.str();
}

std::string ValidationReport::to_json() const {
    std::ostringstream oss;
    oss << "{\"issues\":[";
    for (size_t i = 0; i < issues_.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "{\"code\":\"" << issues_[i].code << "\",\"severity\":" << static_cast<int>(issues_[i].severity) << "}";
    }
    oss << "]}";
    return oss.str();
}

void ValidationReport::merge(const ValidationReport& other) {
    for (const auto& issue : other.issues_) {
        add_issue(issue);
    }
}

void InputValidator::add_range_check(const std::string& param, double min, double max, const std::string& unit) {
    range_checks_.push_back(PlausibilityCheck{param, RangeCheck{min, max, unit}, "Range check"});
}

void InputValidator::add_required(const std::string& param) {
    required_fields_.push_back(param);
}

void InputValidator::add_custom(const std::string& param, std::function<bool(double)> check) {
    custom_checks_[param] = std::move(check);
}

ValidationReport InputValidator::validate(const std::map<std::string, double>& values) const {
    ValidationReport report;
    for (const auto& req : required_fields_) {
        if (values.find(req) == values.end()) {
            report.add_issue("MISSING_FIELD", "Required field missing: " + req, Severity::Error);
        }
    }
    for (const auto& check : range_checks_) {
        auto it = values.find(check.parameter);
        if (it != values.end()) {
            if (it->second < check.range.min || it->second > check.range.max) {
                report.add_issue("OUT_OF_RANGE",
                    check.parameter + " value " + std::to_string(it->second) +
                    " outside [" + std::to_string(check.range.min) + ", " +
                    std::to_string(check.range.max) + "]", Severity::Error);
            }
        }
    }
    return report;
}

ValidationReport InputValidator::validate_schema(const std::map<std::string, std::string>& fields) const {
    ValidationReport report;
    for (const auto& req : required_fields_) {
        if (fields.find(req) == fields.end()) {
            report.add_issue("MISSING_FIELD", "Required field missing: " + req, Severity::Error);
        }
    }
    return report;
}

double DataQualityMetrics::overall() const {
    return (completeness + accuracy + timeliness + consistency + validity) / 5.0;
}

std::string DataQualityMetrics::to_string() const {
    std::ostringstream oss;
    oss << "Quality{completeness=" << completeness
        << ", accuracy=" << accuracy
        << ", timeliness=" << timeliness
        << ", consistency=" << consistency
        << ", validity=" << validity
        << ", overall=" << overall() << "}";
    return oss.str();
}

} // namespace etwin::core::quality
