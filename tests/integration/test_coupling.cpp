#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "coupling/coupling_service.hpp"
#include "coupling/regridding.hpp"
#include "coupling/time_interpolation.hpp"

using namespace etwin::core;
using namespace etwin::coupling;
using namespace etwin::state;

TEST_CASE("Coupling link management", "[coupling]") {
    CouplingService service;
    CouplingLink link{"atmosphere", "temperature", "hydrology", "temperature", CouplingMode::OneWay};
    service.add_link(link);
    REQUIRE(service.links().size() == 1);

    service.remove_link("atmosphere", "hydrology");
    REQUIRE(service.links().empty());
}

TEST_CASE("Coupling unit check", "[coupling]") {
    CouplingService service;
    CouplingLink link{"a", "f", "b", "f", CouplingMode::OneWay};
    auto result = service.check_units(link, units::kelvin, units::celsius);
    REQUIRE(result.passed);
}

TEST_CASE("Coupling unit mismatch", "[coupling]") {
    CouplingService service;
    CouplingLink link{"a", "f", "b", "f", CouplingMode::OneWay};
    auto result = service.check_units(link, units::kelvin, units::meter);
    REQUIRE_FALSE(result.passed);
    REQUIRE(!result.issues.empty());
}

TEST_CASE("Coupling timestep check", "[coupling]") {
    CouplingService service;
    CouplingLink link{"a", "f", "b", "f", CouplingMode::OneWay};
    auto result = service.check_timestep(link, Nanoseconds(3600000000000LL), Nanoseconds(1800000000000LL));
    REQUIRE(result.passed);
}

TEST_CASE("Coupling validate_all", "[coupling]") {
    CouplingService service;
    CouplingLink link{"", "f", "b", "f", CouplingMode::OneWay};
    service.add_link(link);
    REQUIRE_THROWS_AS(service.validate_all(), std::runtime_error);
}

TEST_CASE("Time interpolation linear", "[coupling]") {
    Field before;
    before.data = {0.0};
    before.timestamp = Instant::from_unix_seconds(0.0);

    Field after;
    after.data = {10.0};
    after.timestamp = Instant::from_unix_seconds(100.0);

    TimeInterpolationService interp;
    auto mid = interp.linear(before, after, Instant::from_unix_seconds(50.0));
    REQUIRE(mid.data[0] == Catch::Approx(5.0));
}

TEST_CASE("Mass conservation error", "[coupling]") {
    RegriddingService regrid;
    Field source;
    source.data = {1.0, 1.0, 1.0, 1.0};
    Field target;
    target.data = {1.0, 1.0, 1.0, 1.0};
    REQUIRE(regrid.mass_conservation_error(source, target) == Catch::Approx(0.0));
}
