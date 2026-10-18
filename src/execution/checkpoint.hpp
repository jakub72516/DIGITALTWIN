#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <filesystem>

#include "core/time.hpp"
#include "core/provenance.hpp"
#include "state/earth_state.hpp"

namespace etwin::execution {

struct CheckpointData {
    etwin::core::Instant time;
    uint64_t step_number;
    etwin::state::EarthState state;
    std::map<std::string, std::vector<double>> model_states;
    std::map<std::string, uint64_t> rng_states;
    etwin::core::Hash integrity_hash;
};

class CheckpointManager {
public:
    explicit CheckpointManager(std::string directory);

    bool save(const CheckpointData& checkpoint);
    std::optional<CheckpointData> load(const etwin::core::Instant& t) const;
    std::optional<CheckpointData> load_latest() const;

    std::vector<etwin::core::Instant> available_checkpoints() const;

    bool verify_integrity(const CheckpointData& checkpoint) const;
    void remove_before(const etwin::core::Instant& t);

    const std::string& directory() const { return directory_; }

private:
    std::string directory_;
    std::string path_for(const etwin::core::Instant& t) const;
};

} // namespace etwin::execution
