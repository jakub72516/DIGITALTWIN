#include "security/integrity.hpp"

#include <fstream>
#include <sstream>

namespace etwin::security {

IntegrityReport IntegrityVerifier::verify_file(const std::string& path, const etwin::core::Hash& expected) const {
    IntegrityReport report;
    report.resource = path;
    report.expected_hash = expected;
    report.actual_hash = compute_file_hash(path);
    report.valid = constant_time_compare(report.actual_hash, expected);
    report.message = report.valid ? "integrity verified" : "integrity mismatch";
    return report;
}

IntegrityReport IntegrityVerifier::verify_data(const std::string& data, const etwin::core::Hash& expected) const {
    IntegrityReport report;
    report.resource = "in-memory";
    report.expected_hash = expected;
    report.actual_hash = etwin::core::sha256(data);
    report.valid = constant_time_compare(report.actual_hash, expected);
    report.message = report.valid ? "integrity verified" : "integrity mismatch";
    return report;
}

IntegrityReport IntegrityVerifier::verify_manifest(const std::string& manifest_json, const etwin::core::Hash& expected) const {
    return verify_data(manifest_json, expected);
}

etwin::core::Hash IntegrityVerifier::compute_file_hash(const std::string& path) const {
    return etwin::core::sha256_file(path);
}

bool IntegrityVerifier::constant_time_compare(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) return false;
    unsigned char result = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        result |= static_cast<unsigned char>(a[i] ^ b[i]);
    }
    return result == 0;
}

bool CheckpointIntegrity::verify_checkpoint(const std::string& path, const etwin::core::Hash& expected) const {
    IntegrityVerifier verifier;
    auto report = verifier.verify_file(path, expected);
    return report.valid;
}

etwin::core::Hash CheckpointIntegrity::hash_checkpoint_state(const std::string& state_json) {
    return etwin::core::sha256(state_json);
}

} // namespace etwin::security
