#include "execution/checkpoint.hpp"

#include <nlohmann/json.hpp>
#include <fstream>
#include <algorithm>

namespace etwin::execution {

CheckpointManager::CheckpointManager(std::string directory) : directory_(std::move(directory)) {
    std::filesystem::create_directories(directory_);
}

std::string CheckpointManager::path_for(const etwin::core::Instant& t) const {
    return (std::filesystem::path(directory_) /
        ("checkpoint_" + std::to_string(t.nanoseconds_since_epoch()) + ".json")).string();
}

bool CheckpointManager::save(const CheckpointData& checkpoint) {
    nlohmann::json j;
    j["time_ns"] = checkpoint.time.nanoseconds_since_epoch();
    j["step_number"] = checkpoint.step_number;
    j["integrity_hash"] = checkpoint.integrity_hash;

    std::ofstream f(path_for(checkpoint.time));
    if (!f) return false;
    f << j.dump(2);
    return true;
}

std::optional<CheckpointData> CheckpointManager::load(const etwin::core::Instant& t) const {
    std::ifstream f(path_for(t));
    if (!f) return std::nullopt;

    nlohmann::json j;
    f >> j;

    CheckpointData cp;
    cp.time = etwin::core::Instant(j.value("time_ns", int64_t{0}));
    cp.step_number = j.value("step_number", uint64_t{0});
    cp.integrity_hash = j.value("integrity_hash", "");
    return cp;
}

std::optional<CheckpointData> CheckpointManager::load_latest() const {
    auto times = available_checkpoints();
    if (times.empty()) return std::nullopt;
    return load(times.back());
}

std::vector<etwin::core::Instant> CheckpointManager::available_checkpoints() const {
    std::vector<etwin::core::Instant> result;
    for (const auto& entry : std::filesystem::directory_iterator(directory_)) {
        if (entry.path().extension() != ".json") continue;
        std::string stem = entry.path().stem().string();
        if (stem.rfind("checkpoint_", 0) != 0) continue;
        std::string ns_str = stem.substr(std::string("checkpoint_").size());
        try {
            int64_t ns = std::stoll(ns_str);
            result.push_back(etwin::core::Instant(ns));
        } catch (...) {
            continue;
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}

bool CheckpointManager::verify_integrity(const CheckpointData& checkpoint) const {
    return !checkpoint.integrity_hash.empty();
}

void CheckpointManager::remove_before(const etwin::core::Instant& t) {
    for (const auto& entry : std::filesystem::directory_iterator(directory_)) {
        if (entry.path().extension() != ".json") continue;
        std::string stem = entry.path().stem().string();
        if (stem.rfind("checkpoint_", 0) != 0) continue;
        std::string ns_str = stem.substr(std::string("checkpoint_").size());
        try {
            int64_t ns = std::stoll(ns_str);
            if (ns < t.nanoseconds_since_epoch()) {
                std::filesystem::remove(entry.path());
            }
        } catch (...) {
            continue;
        }
    }
}

} // namespace etwin::execution
