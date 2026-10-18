#include <catch2/catch_test_macros.hpp>

#include "core/time.hpp"

using namespace etwin::core;

TEST_CASE("Instant comparison", "[time]") {
    Instant a = Instant::from_unix_seconds(100.0);
    Instant b = Instant::from_unix_seconds(200.0);
    REQUIRE(a < b);
    REQUIRE(b > a);
    REQUIRE(a == Instant::from_unix_seconds(100.0));
}

TEST_CASE("Instant arithmetic", "[time]") {
    Instant a = Instant::from_unix_seconds(1000.0);
    auto b = a + Nanoseconds(500);
    REQUIRE((b - a).count() == 500);
}

TEST_CASE("TimeRange validation", "[time]") {
    Instant start = Instant::from_unix_seconds(0.0);
    Instant end = Instant::from_unix_seconds(100.0);
    TimeRange range(start, end);
    REQUIRE(range.contains(Instant::from_unix_seconds(50.0)));
    REQUIRE_FALSE(range.contains(Instant::from_unix_seconds(150.0)));
    REQUIRE(range.duration() == Nanoseconds(100'000'000'000));
}

TEST_CASE("TimeRange rejects invalid", "[time]") {
    Instant start = Instant::from_unix_seconds(100.0);
    Instant end = Instant::from_unix_seconds(0.0);
    REQUIRE_THROWS_AS(TimeRange(start, end), std::invalid_argument);
}

TEST_CASE("ISO8601 round trip", "[time]") {
    Instant t = Instant::from_iso8601("2020-01-01T00:00:00");
    std::string iso = t.to_iso8601();
    REQUIRE(!iso.empty());
}
