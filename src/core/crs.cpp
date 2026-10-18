#include "core/crs.hpp"
#include <stdexcept>
#include <cmath>
#include <numbers>
#include <format>

#ifdef ETWIN_HAVE_PROJ
#include <proj.h>
#endif

namespace etwin::core {

#ifdef ETWIN_HAVE_PROJ

CRS::CRS(std::string authority, int code)
    : authority_(std::move(authority)), code_(code) {
    PJ_CONTEXT* ctx = proj_context_create();
    PJ* crs = proj_create(ctx, std::format("{}:{}", authority_, code_).c_str());
    if (!crs) {
        proj_context_destroy(ctx);
        throw std::invalid_argument(std::format("Invalid CRS: {}:{}", authority_, code_));
    }

    const char* wkt_cstr = proj_as_wkt(ctx, crs, PJ_WKT2_2018, nullptr);
    if (wkt_cstr) wkt_ = wkt_cstr;

    const char* name_cstr = proj_get_name(crs);
    if (name_cstr) name_ = name_cstr;

    is_geographic_ = proj_get_type(crs) == PJ_TYPE_GEOGRAPHIC_CRS;
    if (is_geographic_) units_ = "degree";

    proj_destroy(crs);
    proj_context_destroy(ctx);
}

CRS::CRS(std::string wkt) : wkt_(std::move(wkt)) {
    PJ_CONTEXT* ctx = proj_context_create();
    PJ* crs = proj_create(ctx, wkt_.c_str());
    if (!crs) {
        proj_context_destroy(ctx);
        throw std::invalid_argument("Invalid WKT CRS");
    }

    const char* auth = proj_get_id_auth_name(crs);
    const char* code = proj_get_id_code(crs);
    if (auth && code) {
        authority_ = auth;
        code_ = std::stoi(code);
    }

    name_ = proj_get_name(crs) ? proj_get_name(crs) : "unknown";
    is_geographic_ = proj_get_type(crs) == PJ_TYPE_GEOGRAPHIC_CRS;
    if (is_geographic_) units_ = "degree";

    proj_destroy(crs);
    proj_context_destroy(ctx);
}

#else

CRS::CRS(std::string authority, int code)
    : authority_(std::move(authority)), code_(code) {
    name_ = std::format("{}:{}", authority_, code_);
    is_geographic_ = (code_ == 4326);
    if (is_geographic_) units_ = "degree";
    else units_ = "meter";
}

CRS::CRS(std::string wkt) : wkt_(std::move(wkt)) {
    name_ = "custom";
    is_geographic_ = false;
    units_ = "meter";
}

#endif

bool CRS::is_geographic() const { return is_geographic_; }
bool CRS::is_projected() const { return !is_geographic_; }

CRS CRS::epsg_4326() { return CRS("EPSG", 4326); }
CRS CRS::epsg_3857() { return CRS("EPSG", 3857); }
CRS CRS::epsg_32633() { return CRS("EPSG", 32633); }

CRSTransform::CRSTransform(const CRS& source, const CRS& target)
    : source_(source), target_(target) {
#ifdef ETWIN_HAVE_PROJ
    if (source == target) return;

    PJ_CONTEXT* ctx = proj_context_create();
    PJ* transform = proj_create_crs_to_crs(ctx,
        std::format("{}:{}", source_.authority(), source_.code()).c_str(),
        std::format("{}:{}", target_.authority(), target_.code()).c_str(), nullptr);
    if (!transform) {
        proj_context_destroy(ctx);
        throw std::runtime_error("Failed to create CRS transform");
    }
    proj_transform_ = transform;
#else
    (void)source_;
    (void)target_;
#endif
}

CRSTransform::Point CRSTransform::forward(const Point& p) const {
#ifdef ETWIN_HAVE_PROJ
    if (is_identity()) return p;
    if (!proj_transform_) return p;
    PJ_COORD in = proj_coord(p.x, p.y, 0, 0);
    PJ_COORD out = proj_trans(static_cast<PJ*>(proj_transform_), PJ_FWD, in);
    return Point{out.xyzt.x, out.xyzt.y};
#else
    (void)p;
    return p;
#endif
}

CRSTransform::Point CRSTransform::inverse(const Point& p) const {
#ifdef ETWIN_HAVE_PROJ
    if (is_identity()) return p;
    if (!proj_transform_) return p;
    PJ_COORD in = proj_coord(p.x, p.y, 0, 0);
    PJ_COORD out = proj_trans(static_cast<PJ*>(proj_transform_), PJ_INV, in);
    return Point{out.xyzt.x, out.xyzt.y};
#else
    (void)p;
    return p;
#endif
}

bool CRSTransform::is_identity() const {
    return source_ == target_;
}

bool GeoRegion::contains(const GeoPoint& p) const {
    return p.latitude >= min_lat && p.latitude <= max_lat &&
           p.longitude >= min_lon && p.longitude <= max_lon;
}

GeoPoint GeoRegion::center() const {
    return GeoPoint{(min_lat + max_lat) / 2.0, (min_lon + max_lon) / 2.0, 0.0};
}

double GeoRegion::area_km2() const {
    return std::abs(max_lat - min_lat) * 111.0 * std::abs(max_lon - min_lon) * 111.0;
}

std::vector<GeoPoint> GeoRegion::corners() const {
    return {
        GeoPoint{min_lat, min_lon, 0.0},
        GeoPoint{min_lat, max_lon, 0.0},
        GeoPoint{max_lat, max_lon, 0.0},
        GeoPoint{max_lat, min_lon, 0.0},
    };
}

GridDefinition::GridDefinition(const CRS& crs, GridType type, int n_lat, int n_lon,
                               const GeoRegion& region)
    : crs_(crs), type_(type), n_lat_(n_lat), n_lon_(n_lon), region_(region) {}

GeoPoint GridDefinition::cell_center(int i, int j) const {
    double dlat = (region_.max_lat - region_.min_lat) / static_cast<double>(n_lat_);
    double dlon = (region_.max_lon - region_.min_lon) / static_cast<double>(n_lon_);
    double lat = region_.min_lat + (static_cast<double>(i) + 0.5) * dlat;
    double lon = region_.min_lon + (static_cast<double>(j) + 0.5) * dlon;
    return GeoPoint{lat, lon, 0.0};
}

double GridDefinition::cell_area_km2(int i, int j) const {
    double dlat = (region_.max_lat - region_.min_lat) / static_cast<double>(n_lat_);
    double dlon = (region_.max_lon - region_.min_lon) / static_cast<double>(n_lon_);
    GeoPoint center = cell_center(i, j);
    double lat_scale = std::cos(center.latitude * std::numbers::pi / 180.0);
    return std::abs(dlat) * 111.0 * std::abs(dlon) * 111.0 * lat_scale;
}

} // namespace etwin::core
