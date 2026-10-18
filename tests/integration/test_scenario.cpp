#include <catch2/catch_test_macros.hpp>

#include "scenario/scenario_engine.hpp"
#include "scenario/manifest.hpp"
#include "scenario/intervention.hpp"

using namespace etwin::core;
using namespace etwin::scenario;

TEST_CASE("Scenario basic run", "[scenario]") {
    ScenarioConfig config;
    config.scenario_id = "test_scenario";
    config.start_time = Instant::from_unix_seconds(0.0);
    config.end_time = Instant::from_unix_seconds(86400.0);
    config.time_step = Nanoseconds(3600000000000LL);

    ScenarioEngine engine;
    auto result = engine.run(config);
    REQUIRE(result.success);
    REQUIRE(result.scenario_id == "test_scenario");
}

TEST_CASE("Scenario rejects invalid time range", "[scenario]") {
    ScenarioConfig config;
    config.scenario_id = "bad";
    config.start_time = Instant::from_unix_seconds(100.0);
    config.end_time = Instant::from_unix_seconds(0.0);

    ScenarioEngine engine;
    auto result = engine.run(config);
    REQUIRE_FALSE(result.success);
    REQUIRE(!result.errors.empty());
}

TEST_CASE("Scenario unknown model fails", "[scenario]") {
    ScenarioConfig config;
    config.scenario_id = "unknown_model";
    config.start_time = Instant::from_unix_seconds(0.0);
    config.end_time = Instant::from_unix_seconds(100.0);
    config.time_step = Nanoseconds(10000000000LL);

    ModelConfig mc;
    mc.model_name = "does_not_exist";
    config.models.push_back(mc);

    ScenarioEngine engine;
    auto result = engine.run(config);
    REQUIRE_FALSE(result.success);
}

TEST_CASE("Run manifest round trip", "[scenario]") {
    RunManifest m("test_run");
    m.set_initial_state_hash("abc123");
    RunManifest::ModelEntry entry;
    entry.id = "atmosphere";
    entry.version = "1.0";
    entry.seed = 42;
    m.add_model(entry);

    RunManifest::ForcingEntry fe;
    fe.dataset_id = "era5";
    fe.hash = "def456";
    m.add_forcing(fe);

    std::string json = m.to_json();
    RunManifest m2 = RunManifest::from_json(json);
    REQUIRE(m2.scenario_id() == "test_run");
    REQUIRE(m2.models().size() == 1);
    REQUIRE(m2.models()[0].id == "atmosphere");
    REQUIRE(m2.forcing().size() == 1);
}

TEST_CASE("Manifest signing", "[scenario]") {
    RunManifest m("signed_run");
    m.set_initial_state_hash("hash");
    m.sign("private_key");
    REQUIRE(m.is_signed());
    REQUIRE(m.verify("private_key"));
    REQUIRE_FALSE(m.verify("wrong_key"));
}

TEST_CASE("Intervention library", "[scenario]") {
    InterventionLibrary lib;
    Intervention i;
    i.name = "carbon_tax";
    i.target_model = "energy";
    i.parameter = "emission_intensity_fossil";
    i.value = 0.2;
    i.start_time = Instant::from_unix_seconds(0.0);
    lib.add(i);

    REQUIRE(lib.get("carbon_tax") != nullptr);
    REQUIRE(lib.all().size() == 1);
    REQUIRE(lib.active_at(Instant::from_unix_seconds(100.0)).size() == 1);
}
