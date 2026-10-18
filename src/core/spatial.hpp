#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <algorithm>
#include <numbers>
#include <format>
#include <map>
#include <optional>

#include "crs.hpp"

namespace etwin::core::spatial {

constexpr double EARTH_RADIUS_KM = 6371.0;

struct LatLon {
    double lat;
    double lon;

    double distance_km(const LatLon& other) const;
    double bearing_to(const LatLon& other) const;
};

double haversine_km(double lat1, double lon1, double lat2, double lon2);
double vincenty_km(double lat1, double lon1, double lat2, double lon2);

struct BoundingBox {
    double min_lat, max_lat, min_lon, max_lon;

    bool contains(double lat, double lon) const;
    bool intersects(const BoundingBox& other) const;
    BoundingBox intersection(const BoundingBox& other) const;
    double area_km2() const;
};

class H3Index {
public:
    static constexpr int MAX_RESOLUTION = 15;

    H3Index() = default;
    H3Index(uint64_t index) : index_(index) {}

    uint64_t value() const { return index_; }

    static H3Index from_latlon(double lat, double lon, int resolution);
    LatLon to_latlon() const;
    int resolution() const;

    std::vector<H3Index> neighbors() const;
    std::vector<H3Index> children(int target_resolution) const;
    std::optional<H3Index> parent(int target_resolution) const;

    bool operator==(const H3Index&) const = default;
    auto operator<=>(const H3Index&) const = default;

private:
    uint64_t index_ = 0;
};

class SpatialIndex {
public:
    void insert(const H3Index& cell, const std::string& field_id);
    void remove(const H3Index& cell, const std::string& field_id);

    std::vector<std::string> query_region(const GeoRegion& region, int resolution) const;
    std::vector<std::string> query_point(const GeoPoint& point, int resolution) const;

private:
    std::map<uint64_t, std::vector<std::string>> index_;
};

enum class RegridMethod {
    NearestNeighbor,
    Bilinear,
    AreaWeighted,
    Conservative,
};

struct RegridOptions {
    RegridMethod method = RegridMethod::Bilinear;
    double threshold = 1e-6;
    bool normalize_mass = false;
};

} // namespace etwin::core::spatial
