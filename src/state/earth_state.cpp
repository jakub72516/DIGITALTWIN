#include "state/earth_state.hpp"

#include <algorithm>
#include <stdexcept>
#include <limits>
#include <cmath>

namespace etwin::state {

std::string Field::kind_string() const {
    switch (metadata.kind) {
        case FieldKind::Observation: return "observation";
        case FieldKind::Derived: return "derived";
        case FieldKind::Simulation: return "simulation";
    }
    return "unknown";
}

EarthState::EarthState() : version_("0") {}

EarthState::EarthState(std::string version) : version_(std::move(version)) {}

void EarthState::add_field(const Field& field) {
    if (field.metadata.name.empty()) {
        throw std::invalid_argument("Field requires a name");
    }
    fields_[field.metadata.name].push_back(field);
}

void EarthState::remove_field(const std::string& name, const etwin::core::Instant& t) {
    auto it = fields_.find(name);
    if (it == fields_.end()) return;
    auto& vec = it->second;
    vec.erase(std::remove_if(vec.begin(), vec.end(),
        [&t](const Field& f) { return f.timestamp == t; }), vec.end());
    if (vec.empty()) fields_.erase(it);
}

void EarthState::update_field(const Field& field) {
    auto it = fields_.find(field.metadata.name);
    if (it == fields_.end()) {
        add_field(field);
        return;
    }
    auto& vec = it->second;
    for (auto& existing : vec) {
        if (existing.timestamp == field.timestamp) {
            existing = field;
            return;
        }
    }
    vec.push_back(field);
}

std::optional<Field> EarthState::get(const std::string& name, const etwin::core::Instant& t) const {
    auto it = fields_.find(name);
    if (it == fields_.end()) return std::nullopt;

    const auto& vec = it->second;
    const Field* best = nullptr;
    etwin::core::Nanoseconds best_dt(std::numeric_limits<int64_t>::max());

    for (const auto& f : vec) {
        auto dt = t - f.timestamp;
        auto abs_dt = std::abs(dt.count());
        if (best == nullptr || abs_dt < best_dt.count()) {
            best = &f;
            best_dt = etwin::core::Nanoseconds(abs_dt);
        }
    }
    if (!best) return std::nullopt;
    return *best;
}

std::vector<Field> EarthState::all_fields() const {
    std::vector<Field> result;
    for (const auto& [name, vec] : fields_) {
        result.insert(result.end(), vec.begin(), vec.end());
    }
    return result;
}

std::vector<std::string> EarthState::field_names() const {
    std::vector<std::string> result;
    result.reserve(fields_.size());
    for (const auto& [name, vec] : fields_) result.push_back(name);
    return result;
}

std::vector<etwin::core::Instant> EarthState::timestamps() const {
    std::vector<etwin::core::Instant> result;
    for (const auto& [name, vec] : fields_) {
        for (const auto& f : vec) {
            if (std::find(result.begin(), result.end(), f.timestamp) == result.end()) {
                result.push_back(f.timestamp);
            }
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}

etwin::core::Hash EarthState::compute_hash() const {
    etwin::core::Hash combined = version_;
    for (const auto& [name, vec] : fields_) {
        combined = etwin::core::hash_combine(combined, name);
        for (const auto& f : vec) {
            combined = etwin::core::hash_combine(combined, f.provenance.compute_hash());
        }
    }
    return combined;
}

void StateHistory::checkpoint(const EarthState& state, const etwin::core::Instant& t) {
    checkpoints_[t.nanoseconds_since_epoch()] = state;
}

std::optional<EarthState> StateHistory::restore(const etwin::core::Instant& t) const {
    auto it = checkpoints_.find(t.nanoseconds_since_epoch());
    if (it == checkpoints_.end()) return std::nullopt;
    return it->second;
}

std::vector<etwin::core::Instant> StateHistory::checkpoint_times() const {
    std::vector<etwin::core::Instant> result;
    for (const auto& [t, state] : checkpoints_) {
        result.push_back(etwin::core::Instant(t));
    }
    return result;
}

void StateHistory::prune_before(const etwin::core::Instant& t) {
    for (auto it = checkpoints_.begin(); it != checkpoints_.end();) {
        if (it->first < t.nanoseconds_since_epoch()) {
            it = checkpoints_.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace etwin::state
