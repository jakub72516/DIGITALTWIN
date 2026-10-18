#pragma once

#include <string>
#include <memory>
#include <vector>
#include <compare>
#include <format>

namespace etwin::core {

class CRS {
public:
    CRS() = default;
    CRS(std::string authority, int code);
    explicit CRS(std::string wkt);

    const std::string& authority() const { return authority_; }
    int code() const { return code_; }
    const std::string& wkt() const { return wkt_; }
    const std::string& name() const { return name_; }

    constexpr auto operator<=>(const CRS&) const = default;
    bool operator==(const CRS&) const = default;

    bool is_geographic() const;
    bool is_projected() const;
    const std::string& units() const { return units_; }

    static CRS epsg_4326();
    static CRS epsg_3857();
    static CRS epsg_32633();

private:
    std::string authority_;
    int code_ = 0;
    std::string wkt_;
    std::string name_;
    std::string units_ = "degree";
    bool is_geographic_ = false;
};

class CRSTransform {
public:
    CRSTransform(const CRS& source, const CRS& target);

    struct Point { double x, y; };
    Point forward(const Point& p) const;
    Point inverse(const Point& p) const;

    const CRS& source() const { return source_; }
    const CRS& target() const { return target_; }
    bool is_identity() const;

private:
    CRS source_;
    CRS target_;
    void* proj_transform_ = nullptr;
};

enum class GridType {
    RegularLatLon,
    Curvilinear,
    UnstructuredMesh,
    Point,
};

struct GeoPoint {
    double latitude;
    double longitude;
    double elevation = 0.0;

    constexpr bool operator==(const GeoPoint&) const = default;
};

struct GeoRegion {
    double min_lat;
    double max_lat;
    double min_lon;
    double max_lon;

    bool contains(const GeoPoint& p) const;
    GeoPoint center() const;
    double area_km2() const;
    std::vector<GeoPoint> corners() const;
};

class GridDefinition {
public:
    GridDefinition(const CRS& crs, GridType type, int n_lat, int n_lon,
                   const GeoRegion& region);

    const CRS& crs() const { return crs_; }
    GridType type() const { return type_; }
    int n_lat() const { return n_lat_; }
    int n_lon() const { return n_lon_; }
    const GeoRegion& region() const { return region_; }
    int size() const { return n_lat_ * n_lon_; }

    GeoPoint cell_center(int i, int j) const;
    double cell_area_km2(int i, int j) const;

private:
    CRS crs_;
    GridType type_;
    int n_lat_;
    int n_lon_;
    GeoRegion region_;
};

} // namespace etwin::core
