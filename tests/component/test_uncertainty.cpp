#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "core/uncertainty.hpp"

#include <random>

using namespace etwin::core::uncertainty;

TEST_CASE("Gaussian distribution", "[uncertainty]") {
    auto dist = Distribution::gaussian(0.0, 1.0);
    REQUIRE(dist.mean() == Catch::Approx(0.0));
    REQUIRE(dist.stddev() == Catch::Approx(1.0));
}

TEST_CASE("Uniform distribution", "[uncertainty]") {
    auto dist = Distribution::uniform(0.0, 10.0);
    REQUIRE(dist.mean() == Catch::Approx(5.0));
    std::mt19937_64 rng(42);
    double s = dist.sample(rng);
    REQUIRE(s >= 0.0);
    REQUIRE(s <= 10.0);
}

TEST_CASE("Uncertain value propagation", "[uncertainty]") {
    UncertainValue a(10.0, Distribution::gaussian(10.0, 1.0));
    UncertainValue b(5.0, Distribution::gaussian(5.0, 0.5));
    auto sum = a + b;
    REQUIRE(sum.nominal() == Catch::Approx(15.0));
}

TEST_CASE("Ensemble statistics", "[uncertainty]") {
    Ensemble e(3);
    e.set_member(0, 1.0, Distribution::gaussian(1.0, 0.1));
    e.set_member(1, 2.0, Distribution::gaussian(2.0, 0.1));
    e.set_member(2, 3.0, Distribution::gaussian(3.0, 0.1));
    REQUIRE(e.mean() == Catch::Approx(2.0));
    REQUIRE(e.std_dev() > 0.0);
    REQUIRE(e.quantile(0.5) == Catch::Approx(2.0));
}

TEST_CASE("Uncertainty budget", "[uncertainty]") {
    UncertaintyBudget budget;
    budget.add_component("measurement", 1.0);
    budget.add_component("parameter", 3.0);
    REQUIRE(budget.total_variance() == Catch::Approx(4.0));
    REQUIRE(budget.stddev() == Catch::Approx(2.0));
}
