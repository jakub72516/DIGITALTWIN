#include "security/input_validator.hpp"

#include <algorithm>
#include <cctype>

namespace etwin::security {

void InputValidator::add_range_check(const std::string& field, double min, double max) {
    ranges_[field] = {min, max};
}

void InputValidator::add_required(const std::string& field) {
    required_.push_back(field);
}

void InputValidator::add_allowed_value(const std::string& field, const std::string& value) {
    allowed_values_[field].push_back(value);
}

etwin::core::quality::ValidationReport InputValidator::validate(
    const std::map<std::string, double>& numeric_fields,
    const std::map<std::string, std::string>& string_fields) const {
    etwin::core::quality::ValidationReport report;

    for (const auto& req : required_) {
        if (!numeric_fields.contains(req) && !string_fields.contains(req)) {
            report.add_issue("MISSING_REQUIRED", "Missing required field: " + req,
                etwin::core::quality::Severity::Error);
        }
    }

    for (const auto& [field, range] : ranges_) {
        auto it = numeric_fields.find(field);
        if (it != numeric_fields.end()) {
            if (it->second < range.first || it->second > range.second) {
                report.add_issue("OUT_OF_RANGE", field + " outside allowed range",
                    etwin::core::quality::Severity::Error);
            }
        }
    }

    for (const auto& [field, allowed] : allowed_values_) {
        auto it = string_fields.find(field);
        if (it != string_fields.end()) {
            if (std::find(allowed.begin(), allowed.end(), it->second) == allowed.end()) {
                report.add_issue("INVALID_VALUE", field + " has disallowed value",
                    etwin::core::quality::Severity::Error);
            }
        }
    }

    return report;
}

bool InputValidator::is_safe_string(const std::string& s) {
    for (char c : s) {
        if (c == '\0' || c == '\r' || c == '\n') return false;
    }
    return true;
}

bool InputValidator::is_safe_path(const std::string& s) {
    if (s.empty()) return false;
    if (s.find("..") != std::string::npos) return false;
    if (s.find(':') != std::string::npos && s.find(':') != 1) return false;
    return is_safe_string(s);
}

} // namespace etwin::security
