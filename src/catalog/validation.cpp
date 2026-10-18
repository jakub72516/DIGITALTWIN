#include "catalog/validation.hpp"

#include <algorithm>
#include <cmath>

namespace etwin::catalog::validation {

void SchemaValidator::add_required_field(const std::string& field) {
    required_.push_back(field);
}

void SchemaValidator::add_field_type(const std::string& field, const std::string& type) {
    types_[field] = type;
}

etwin::core::quality::ValidationReport SchemaValidator::validate(
    const std::map<std::string, std::string>& fields) const {
    etwin::core::quality::ValidationReport report;
    for (const auto& req : required_) {
        if (!fields.contains(req)) {
            report.add_issue("MISSING_REQUIRED_FIELD",
                "Missing required field: " + req,
                etwin::core::quality::Severity::Error);
        }
    }
    for (const auto& [field, expected] : types_) {
        auto it = fields.find(field);
        if (it != fields.end() && it->second != expected) {
            report.add_issue("FIELD_TYPE_MISMATCH",
                "Field " + field + " expected type " + expected +
                " but got " + it->second,
                etwin::core::quality::Severity::Warning);
        }
    }
    return report;
}

void RangeValidator::add_range(const std::string& param, double min, double max) {
    ranges_[param] = {min, max};
}

etwin::core::quality::ValidationReport RangeValidator::validate(
    const std::map<std::string, double>& values) const {
    etwin::core::quality::ValidationReport report;
    for (const auto& [param, range] : ranges_) {
        auto it = values.find(param);
        if (it != values.end()) {
            double v = it->second;
            if (std::isnan(v) || v < range.first || v > range.second) {
                report.add_issue("OUT_OF_RANGE",
                    param + " = " + std::to_string(v) + " outside [" +
                    std::to_string(range.first) + ", " + std::to_string(range.second) + "]",
                    etwin::core::quality::Severity::Error);
            }
        }
    }
    return report;
}

bool TemporalConsistencyValidator::has_duplicates(
    const std::vector<etwin::core::Instant>& times) {
    for (size_t i = 1; i < times.size(); ++i) {
        if (times[i] == times[i - 1]) return true;
    }
    return false;
}

bool TemporalConsistencyValidator::has_gaps(
    const std::vector<etwin::core::Instant>& times,
    etwin::core::Nanoseconds expected_dt) {
    if (times.size() < 2 || expected_dt.count() <= 0) return false;
    for (size_t i = 1; i < times.size(); ++i) {
        auto dt = times[i] - times[i - 1];
        if (dt.count() > expected_dt.count() * 2) return true;
    }
    return false;
}

etwin::core::quality::ValidationReport TemporalConsistencyValidator::validate(
    const std::vector<etwin::core::Instant>& times) const {
    etwin::core::quality::ValidationReport report;
    if (has_duplicates(times)) {
        report.add_issue("DUPLICATE_TIMESTAMPS",
            "Duplicate timestamps detected",
            etwin::core::quality::Severity::Error);
    }
    return report;
}

bool SpatialConsistencyValidator::crs_compatible(
    const etwin::core::CRS& a, const etwin::core::CRS& b) {
    return a == b;
}

etwin::core::quality::ValidationReport SpatialConsistencyValidator::validate(
    const etwin::core::GeoRegion& declared,
    const etwin::core::GeoRegion& actual) const {
    etwin::core::quality::ValidationReport report;
    if (actual.min_lat < declared.min_lat - 1e-6 ||
        actual.max_lat > declared.max_lat + 1e-6 ||
        actual.min_lon < declared.min_lon - 1e-6 ||
        actual.max_lon > declared.max_lon + 1e-6) {
        report.add_issue("OUT_OF_COVERAGE",
            "Actual region exceeds declared coverage",
            etwin::core::quality::Severity::Error);
    }
    return report;
}

etwin::core::quality::ValidationReport PipelineValidator::validate_all(
    const std::map<std::string, std::string>& schema,
    const std::map<std::string, double>& values,
    const std::vector<etwin::core::Instant>& times,
    const etwin::core::GeoRegion& declared_region,
    const etwin::core::GeoRegion& actual_region) const {
    etwin::core::quality::ValidationReport report;
    report.merge(schema_.validate(schema));
    report.merge(range_.validate(values));
    report.merge(temporal_.validate(times));
    report.merge(spatial_.validate(declared_region, actual_region));
    return report;
}

} // namespace etwin::catalog::validation
