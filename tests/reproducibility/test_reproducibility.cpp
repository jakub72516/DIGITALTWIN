#include <catch2/catch_test_macros.hpp>

#include "core/provenance.hpp"
#include "core/time.hpp"
#include "scenario/manifest.hpp"

using namespace etwin::core;

TEST_CASE("Deterministic hash", "[reproducibility]") {
    std::string data = "reproducible data";
    Hash h1 = sha256(data);
    Hash h2 = sha256(data);
    REQUIRE(h1 == h2);
}

TEST_CASE("Hash changes with input", "[reproducibility]") {
    REQUIRE(sha256("data1") != sha256("data2"));
}

TEST_CASE("Manifest signature reproducibility", "[reproducibility]") {
    etwin::scenario::RunManifest m1("run1");
    m1.set_initial_state_hash("state_hash");
    Hash sig1 = m1.compute_signature();

    etwin::scenario::RunManifest m2("run1");
    m2.set_initial_state_hash("state_hash");
    Hash sig2 = m2.compute_signature();

    REQUIRE(sig1 == sig2);
}

TEST_CASE("Deterministic instant", "[reproducibility]") {
    Instant t1 = Instant::from_unix_seconds(123456.789);
    Instant t2 = Instant::from_unix_seconds(123456.789);
    REQUIRE(t1 == t2);
    REQUIRE(t1.nanoseconds_since_epoch() == t2.nanoseconds_since_epoch());
}
