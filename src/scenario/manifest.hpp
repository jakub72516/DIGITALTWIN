#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>

#include "core/provenance.hpp"
#include "scenario/scenario_engine.hpp"

namespace etwin::scenario {

class RunManifest {
public:
    struct ModelEntry {
        std::string id;
        std::string version;
        std::map<std::string, double> parameters;
        uint64_t seed;
    };

    struct ForcingEntry {
        std::string dataset_id;
        std::string hash;
    };

    RunManifest();
    explicit RunManifest(std::string scenario_id);

    void add_model(const ModelEntry& entry);
    void add_forcing(const ForcingEntry& entry);
    void add_intervention(const Intervention& intervention);

    const std::string& scenario_id() const { return scenario_id_; }
    const std::vector<ModelEntry>& models() const { return models_; }
    const std::vector<ForcingEntry>& forcing() const { return forcing_; }
    const std::vector<Intervention>& interventions() const { return interventions_; }

    void set_initial_state_hash(const std::string& hash) { initial_state_hash_ = hash; }
    const std::string& initial_state_hash() const { return initial_state_hash_; }

    std::string to_json() const;
    static RunManifest from_json(const std::string& json);

    etwin::core::Hash compute_signature() const;

    void sign(const std::string& private_key);
    bool verify(const std::string& public_key) const;

    bool is_signed() const { return !signature_.empty(); }
    const std::string& signature() const { return signature_; }

private:
    std::string scenario_id_;
    std::vector<ModelEntry> models_;
    std::vector<ForcingEntry> forcing_;
    std::vector<Intervention> interventions_;
    std::string initial_state_hash_;
    std::string signature_;
};

} // namespace etwin::scenario
