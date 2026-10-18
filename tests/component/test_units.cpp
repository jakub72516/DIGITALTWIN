#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "core/units.hpp"

using namespace etwin::core;

TEST_CASE("Unit conversion", "[units]") {
    Quantity<double> temp(25.0, units::celsius);
    REQUIRE(temp.in(units::kelvin).value() == Catch::Approx(298.15));

    Quantity<double> dist(1.0, units::kilometer);
    REQUIRE(dist.in(units::meter).value() == Catch::Approx(1000.0));
}

TEST_CASE("Unit compatibility", "[units]") {
    REQUIRE(units::meter.compatible(units::kilometer));
    REQUIRE_FALSE(units::meter.compatible(units::second));
}

TEST_CASE("Quantity arithmetic", "[units]") {
    Quantity<double> a(10.0, units::meter);
    Quantity<double> b(5.0, units::meter);
    auto sum = a + b;
    REQUIRE(sum.value() == Catch::Approx(15.0));

    Quantity<double> c(2.0, units::second);
    auto ratio = a / c;
    REQUIRE(ratio.unit().dim == units::meter_per_second.dim);
}

TEST_CASE("Unit mismatch throws", "[units]") {
    Quantity<double> a(10.0, units::meter);
    Quantity<double> b(5.0, units::second);
    REQUIRE_THROWS_AS(a + b, UnitMismatchError);
}

TEST_CASE("Dimensionless and percent", "[units]") {
    Quantity<double> p(50.0, units::percent);
    REQUIRE(p.si() == Catch::Approx(0.5));
}
