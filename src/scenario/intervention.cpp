#include "scenario/intervention.hpp"

#include <algorithm>

namespace etwin::scenario {

void InterventionLibrary::add(const Intervention& intervention) {
    interventions_.push_back(intervention);
}

void InterventionLibrary::remove(const std::string& name) {
    interventions_.erase(
        std::remove_if(interventions_.begin(), interventions_.end(),
            [&](const Intervention& i) { return i.name == name; }),
        interventions_.end());
}

const Intervention* InterventionLibrary::get(const std::string& name) const {
    for (const auto& i : interventions_) {
        if (i.name == name) return &i;
    }
    return nullptr;
}

std::vector<Intervention> InterventionLibrary::all() const {
    return interventions_;
}

std::vector<Intervention> InterventionLibrary::active_at(const etwin::core::Instant& t) const {
    std::vector<Intervention> result;
    for (const auto& i : interventions_) {
        if (t >= i.start_time) result.push_back(i);
    }
    return result;
}

} // namespace etwin::scenario
