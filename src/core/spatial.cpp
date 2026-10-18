#include "core/spatial.hpp"
#include <cmath>
#include <algorithm>
#include <set>
#include <numbers>

namespace etwin::core::spatial {

double haversine_km(double lat1, double lon1, double lat2, double lon2) {
    constexpr double R = EARTH_RADIUS_KM;
    double dlat = (lat2 - lat1) * std::numbers::pi / 180.0;
    double dlon = (lon2 - lon1) * std::numbers::pi / 180.0;
    double a = std::sin(dlat / 2) * std::sin(dlat / 2) +
               std::cos(lat1 * std::numbers::pi / 180.0) * std::cos(lat2 * std::numbers::pi / 180.0) *
               std::sin(dlon / 2) * std::sin(dlon / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    return R * c;
}

double vincenty_km(double lat1, double lon1, double lat2, double lon2) {
    return haversine_km(lat1, lon1, lat2, lon2);
}

double LatLon::distance_km(const LatLon& other) const {
    return haversine_km(lat, lon, other.lat, other.lon);
}

double LatLon::bearing_to(const LatLon& other) const {
    double lat1 = lat * std::numbers::pi / 180.0;
    double lat2 = other.lat * std::numbers::pi / 180.0;
    double dlon = (other.lon - lon) * std::numbers::pi / 180.0;
    double y = std::sin(dlon) * std::cos(lat2);
    double x = std::cos(lat1) * std::sin(lat2) - std::sin(lat1) * std::cos(lat2) * std::cos(dlon);
    return std::atan2(y, x) * 180.0 / std::numbers::pi;
}

bool BoundingBox::contains(double lat, double lon) const {
    return lat >= min_lat && lat <= max_lat && lon >= min_lon && lon <= max_lon;
}

bool BoundingBox::intersects(const BoundingBox& other) const {
    return !(other.min_lat > max_lat || other.max_lat < min_lat ||
             other.min_lon > max_lon || other.max_lon < min_lon);
}

BoundingBox BoundingBox::intersection(const BoundingBox& other) const {
    return BoundingBox{
        std::max(min_lat, other.min_lat),
        std::min(max_lat, other.max_lat),
        std::max(min_lon, other.min_lon),
        std::min(max_lon, other.max_lon),
    };
}

double BoundingBox::area_km2() const {
    return haversine_km(min_lat, min_lon, max_lat, min_lon) *
           haversine_km(min_lat, min_lon, min_lat, max_lon);
}

H3Index H3Index::from_latlon(double lat, double lon, int resolution) {
    return H3Index(0);
}

LatLon H3Index::to_latlon() const {
    return LatLon{0.0, 0.0};
}

int H3Index::resolution() const {
    return 0;
}

std::vector<H3Index> H3Index::neighbors() const {
    return {};
}

std::vector<H3Index> H3Index::children(int target_resolution) const {
    return {};
}

std::optional<H3Index> H3Index::parent(int target_resolution) const {
    return std::nullopt;
}

void SpatialIndex::insert(const H3Index& cell, const std::string& field_id) {
    index_[cell.value()].push_back(field_id);
}

void SpatialIndex::remove(const H3Index& cell, const std::string& field_id) {
    auto it = index_.find(cell.value());
    if (it != index_.end()) {
        auto& vec = it->second;
        vec.erase(std::remove(vec.begin(), vec.end(), field_id), vec.end());
        if (vec.empty()) index_.erase(it);
    }
}

std::vector<std::string> SpatialIndex::query_region(const GeoRegion& region, int resolution) const {
    std::set<std::string> results;
    for (const auto& [idx, fields] : index_) {
        for (const auto& f : fields) {
            results.insert(f);
        }
    }
    return std::vector<std::string>(results.begin(), results.end());
}

std::vector<std::string> SpatialIndex::query_point(const GeoPoint& point, int resolution) const {
    return {};
}

} // namespace etwin::core::spatial
