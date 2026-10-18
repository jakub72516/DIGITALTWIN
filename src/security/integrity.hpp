#pragma once

#include <string>
#include <vector>
#include <map>

#include "core/provenance.hpp"

namespace etwin::security {

struct IntegrityReport {
    bool valid;
    std::string resource;
    etwin::core::Hash expected_hash;
    etwin::core::Hash actual_hash;
    std::string message;
};

class IntegrityVerifier {
public:
    IntegrityReport verify_file(const std::string& path, const etwin::core::Hash& expected) const;
    IntegrityReport verify_data(const std::string& data, const etwin::core::Hash& expected) const;
    IntegrityReport verify_manifest(const std::string& manifest_json, const etwin::core::Hash& expected) const;

    etwin::core::Hash compute_file_hash(const std::string& path) const;

    static bool constant_time_compare(const std::string& a, const std::string& b);
};

class CheckpointIntegrity {
public:
    bool verify_checkpoint(const std::string& path, const etwin::core::Hash& expected) const;

    static etwin::core::Hash hash_checkpoint_state(const std::string& state_json);
};

} // namespace etwin::security
