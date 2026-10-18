#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "core/spatial.hpp"
#include "core/crs.hpp"

using namespace etwin::core;

TEST_CASE("Haversine distance", "[spatial]") {
    double d = spatial::haversine_km(0.0, 0.0, 0.0, 1.0);
    REQUIRE(d == Catch::Approx(111.19).epsilon(0.01));
}

TEST_CASE("Bounding box containment", "[spatial]") {
    spatial::BoundingBox box{-10.0, 10.0, -20.0, 20.0};
    REQUIRE(box.contains(0.0, 0.0));
    REQUIRE_FALSE(box.contains(50.0, 0.0));
}

TEST_CASE("Bounding box intersection", "[spatial]") {
    spatial::BoundingBox a{-10.0, 10.0, -20.0, 20.0};
    spatial::BoundingBox b{0.0, 20.0, 0.0, 40.0};
    REQUIRE(a.intersects(b));
    auto inter = a.intersection(b);
    REQUIRE(inter.min_lat == Catch::Approx(0.0));
    REQUIRE(inter.max_lat == Catch::Approx(10.0));
}

TEST_CASE("GeoRegion contains", "[spatial]") {
    GeoRegion region{-90.0, 90.0, -180.0, 180.0};
    REQUIRE(region.contains(GeoPoint{0.0, 0.0, 0.0}));
    REQUIRE(region.center().latitude == Catch::Approx(0.0));
}

TEST_CASE("Spatial index insert/query", "[spatial]") {
    spatial::SpatialIndex index;
    index.insert(spatial::H3Index(1), "field_a");
    index.insert(spatial::H3Index(2), "field_b");
    auto results = index.query_region(GeoRegion{-90.0, 90.0, -180.0, 180.0}, 0);
    REQUIRE(results.size() == 2);
}
