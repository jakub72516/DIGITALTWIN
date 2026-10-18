#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "validation/validator.hpp"
#include "validation/hindcast.hpp"
#include "validation/uncertainty_propagation.hpp"

using namespace etwin::validation;

TEST_CASE("RMSE", "[validation]") {
    ModelValidator v;
    std::vector<double> pred{1.0, 2.0, 3.0};
    std::vector<double> obs{1.0, 2.0, 3.0};
    REQUIRE(v.rmse(pred, obs) == Catch::Approx(0.0));
}

TEST_CASE("RMSE non-zero", "[validation]") {
    ModelValidator v;
    std::vector<double> pred{1.0, 2.0, 3.0};
    std::vector<double> obs{2.0, 3.0, 4.0};
    REQUIRE(v.rmse(pred, obs) == Catch::Approx(1.0));
}

TEST_CASE("Bias", "[validation]") {
    ModelValidator v;
    std::vector<double> pred{1.0, 2.0, 3.0};
    std::vector<double> obs{2.0, 3.0, 4.0};
    REQUIRE(v.bias(pred, obs) == Catch::Approx(-1.0));
}

TEST_CASE("Correlation", "[validation]") {
    ModelValidator v;
    std::vector<double> pred{1.0, 2.0, 3.0, 4.0};
    std::vector<double> obs{1.0, 2.0, 3.0, 4.0};
    REQUIRE(v.correlation(pred, obs) == Catch::Approx(1.0));
}

TEST_CASE("Nash-Sutcliffe perfect", "[validation]") {
    ModelValidator v;
    std::vector<double> pred{1.0, 2.0, 3.0};
    std::vector<double> obs{1.0, 2.0, 3.0};
    REQUIRE(v.nash_sutcliffe(pred, obs) == Catch::Approx(1.0));
}

TEST_CASE("Monte Carlo propagation", "[validation]") {
    UncertaintyPropagator prop(42);
    auto dist = etwin::core::uncertainty::Distribution::uniform(0.0, 10.0);
    std::vector<double> samples = prop.monte_carlo(
        [](const std::vector<double>& x) { return x[0] * 2.0; },
        {dist}, 1000);
    REQUIRE(samples.size() == 1000);
    REQUIRE(prop.mean(samples) == Catch::Approx(10.0).epsilon(0.1));
}

TEST_CASE("CRPS", "[validation]") {
    std::vector<double> ensemble{1.0, 2.0, 3.0};
    double crps = HindcastService::crps(ensemble, 2.0);
    REQUIRE(crps >= 0.0);
}

TEST_CASE("Brier score perfect", "[validation]") {
    std::vector<double> probs{1.0, 1.0};
    std::vector<int> outcomes{1, 1};
    REQUIRE(HindcastService::brier_score(probs, outcomes) == Catch::Approx(0.0));
}
