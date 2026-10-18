#include "scenario/manifest.hpp"

#include <nlohmann/json.hpp>
#include <sstream>

namespace etwin::scenario {

RunManifest::RunManifest() = default;
RunManifest::RunManifest(std::string scenario_id) : scenario_id_(std::move(scenario_id)) {}

void RunManifest::add_model(const ModelEntry& entry) { models_.push_back(entry); }
void RunManifest::add_forcing(const ForcingEntry& entry) { forcing_.push_back(entry); }
void RunManifest::add_intervention(const Intervention& intervention) {
    interventions_.push_back(intervention);
}

std::string RunManifest::to_json() const {
    nlohmann::json j;
    j["scenario_id"] = scenario_id_;
    j["initial_state_hash"] = initial_state_hash_;

    j["models"] = nlohmann::json::array();
    for (const auto& m : models_) {
        nlohmann::json e;
        e["id"] = m.id;
        e["version"] = m.version;
        e["seed"] = m.seed;
        e["parameters"] = m.parameters;
        j["models"].push_back(e);
    }

    j["forcing"] = nlohmann::json::array();
    for (const auto& f : forcing_) {
        nlohmann::json e;
        e["dataset_id"] = f.dataset_id;
        e["hash"] = f.hash;
        j["forcing"].push_back(e);
    }

    j["interventions"] = nlohmann::json::array();
    for (const auto& i : interventions_) {
        nlohmann::json e;
        e["name"] = i.name;
        e["description"] = i.description;
        e["target_model"] = i.target_model;
        e["parameter"] = i.parameter;
        e["value"] = i.value;
        j["interventions"].push_back(e);
    }

    return j.dump(2);
}

RunManifest RunManifest::from_json(const std::string& json) {
    nlohmann::json j = nlohmann::json::parse(json);
    RunManifest m(j.value("scenario_id", ""));
    m.initial_state_hash_ = j.value("initial_state_hash", "");

    for (const auto& e : j.value("models", nlohmann::json::array())) {
        ModelEntry entry;
        entry.id = e.value("id", "");
        entry.version = e.value("version", "");
        entry.seed = e.value("seed", 0);
        if (e.contains("parameters")) entry.parameters = e["parameters"].get<std::map<std::string, double>>();
        m.add_model(entry);
    }

    for (const auto& e : j.value("forcing", nlohmann::json::array())) {
        ForcingEntry entry;
        entry.dataset_id = e.value("dataset_id", "");
        entry.hash = e.value("hash", "");
        m.add_forcing(entry);
    }

    for (const auto& e : j.value("interventions", nlohmann::json::array())) {
        Intervention i;
        i.name = e.value("name", "");
        i.description = e.value("description", "");
        i.target_model = e.value("target_model", "");
        i.parameter = e.value("parameter", "");
        i.value = e.value("value", 0.0);
        m.add_intervention(i);
    }

    return m;
}

etwin::core::Hash RunManifest::compute_signature() const {
    return etwin::core::sha256(to_json());
}

void RunManifest::sign(const std::string& private_key) {
    signature_ = etwin::core::sha256(private_key + compute_signature());
}

bool RunManifest::verify(const std::string& public_key) const {
    if (signature_.empty()) return false;
    return signature_ == etwin::core::sha256(public_key + compute_signature());
}

} // namespace etwin::scenario
