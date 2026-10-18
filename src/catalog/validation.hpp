#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>

#include "core/quality.hpp"
#include "core/time.hpp"
#include "core/crs.hpp"

namespace etwin::catalog::validation {

class SchemaValidator {
public:
    void add_required_field(const std::string& field);
    void add_field_type(const std::string& field, const std::string& type);

    etwin::core::quality::ValidationReport validate(
        const std::map<std::string, std::string>& fields) const;

private:
    std::vector<std::string> required_;
    std::map<std::string, std::string> types_;
};

class RangeValidator {
public:
    void add_range(const std::string& param, double min, double max);

    etwin::core::quality::ValidationReport validate(
        const std::map<std::string, double>& values) const;

private:
    std::map<std::string, std::pair<double, double>> ranges_;
};

class TemporalConsistencyValidator {
public:
    etwin::core::quality::ValidationReport validate(
        const std::vector<etwin::core::Instant>& times) const;

    static bool has_gaps(const std::vector<etwin::core::Instant>& times,
                         etwin::core::Nanoseconds expected_dt);
    static bool has_duplicates(const std::vector<etwin::core::Instant>& times);
};

class SpatialConsistencyValidator {
public:
    etwin::core::quality::ValidationReport validate(
        const etwin::core::GeoRegion& declared,
        const etwin::core::GeoRegion& actual) const;

    static bool crs_compatible(const etwin::core::CRS& a, const etwin::core::CRS& b);
};

class PipelineValidator {
public:
    etwin::core::quality::ValidationReport validate_all(
        const std::map<std::string, std::string>& schema,
        const std::map<std::string, double>& values,
        const std::vector<etwin::core::Instant>& times,
        const etwin::core::GeoRegion& declared_region,
        const etwin::core::GeoRegion& actual_region) const;

private:
    SchemaValidator schema_;
    RangeValidator range_;
    TemporalConsistencyValidator temporal_;
    SpatialConsistencyValidator spatial_;
};

} // namespace etwin::catalog::validation
