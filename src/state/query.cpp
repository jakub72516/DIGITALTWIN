#include "state/query.hpp"

namespace etwin::state {

std::optional<Field> EarthStateQuery::at(const etwin::core::GeoPoint& point,
                                         const etwin::core::Instant& t,
                                         const std::string& field_name) const {
    auto field = state_.get(field_name, t);
    if (!field) return std::nullopt;

    if (field->metadata.kind == FieldKind::Observation) {
        return field;
    }
    return field;
}

std::vector<Field> EarthStateQuery::over(const etwin::core::GeoRegion& region,
                                         const etwin::core::TimeRange& range,
                                         const std::string& field_name) const {
    std::vector<Field> result;
    for (const auto& f : state_.all_fields()) {
        if (f.metadata.name != field_name) continue;
        if (f.timestamp < range.start() || f.timestamp >= range.end()) continue;
        result.push_back(f);
    }
    return result;
}

std::vector<Field> EarthStateQuery::by_kind(FieldKind kind) const {
    std::vector<Field> result;
    for (const auto& f : state_.all_fields()) {
        if (f.metadata.kind == kind) result.push_back(f);
    }
    return result;
}

std::vector<Field> EarthStateQuery::with_quality_above(double threshold) const {
    std::vector<Field> result;
    for (const auto& f : state_.all_fields()) {
        if (f.quality.confidence >= threshold) result.push_back(f);
    }
    return result;
}

bool EarthStateQuery::has_coverage(const etwin::core::GeoPoint& point,
                                   const std::string& field_name) const {
    auto field = state_.get(field_name, etwin::core::Instant::now());
    return field.has_value();
}

} // namespace etwin::state
