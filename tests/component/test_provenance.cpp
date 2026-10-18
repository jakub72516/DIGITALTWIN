#include <catch2/catch_test_macros.hpp>

#include "core/provenance.hpp"

using namespace etwin::core;

TEST_CASE("SHA256 deterministic", "[provenance]") {
    std::string a = sha256("hello world");
    std::string b = sha256("hello world");
    REQUIRE(a == b);
    REQUIRE(a.size() == 64);
}

TEST_CASE("SHA256 differs", "[provenance]") {
    REQUIRE(sha256("hello world") != sha256("hello world!"));
}

TEST_CASE("Hash combine", "[provenance]") {
    Hash h = hash_combine("abc", "def");
    REQUIRE(!h.empty());
}

TEST_CASE("Provenance hash", "[provenance]") {
    DataSource src;
    src.dataset_id = "test";
    src.version = "1.0";
    src.content_hash = "abc123";
    Provenance p(src);
    REQUIRE(p.compute_hash() != "");
}

TEST_CASE("Provenance reproducibility", "[provenance]") {
    DataSource src;
    src.dataset_id = "test";
    src.version = "1.0";
    src.retrieval_time = Instant::now();
    src.content_hash = "abc123";
    Provenance p(src);
    REQUIRE(p.is_reproducible());
}

TEST_CASE("Provenance with issues", "[provenance]") {
    DataSource src;
    src.dataset_id = "test";
    src.version = "1.0";
    Provenance p(src);
    QualityFlags qf;
    qf.gap_filled = true;
    p.set_quality(qf);
    REQUIRE(p.has_known_issues());
}
