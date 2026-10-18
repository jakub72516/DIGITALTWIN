#pragma once

#include <string>
#include <vector>
#include <optional>

#include "core/time.hpp"
#include "core/crs.hpp"
#include "core/spatial.hpp"

#include "state/earth_state.hpp"

namespace etwin::state {

class EarthStateQuery {
public:
    explicit EarthStateQuery(const EarthState& state) : state_(state) {}

    std::optional<Field> at(const etwin::core::GeoPoint& point,
                            const etwin::core::Instant& t,
                            const std::string& field_name) const;

    std::vector<Field> over(const etwin::core::GeoRegion& region,
                            const etwin::core::TimeRange& range,
                            const std::string& field_name) const;

    std::vector<Field> by_kind(FieldKind kind) const;

    std::vector<Field> with_quality_above(double threshold) const;

    bool has_coverage(const etwin::core::GeoPoint& point, const std::string& field_name) const;

private:
    const EarthState& state_;
};

} // namespace etwin::state
