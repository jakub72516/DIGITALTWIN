#pragma once

#include <string>
#include <vector>

#include "core/time.hpp"
#include "scenario/scenario_engine.hpp"

namespace etwin::scenario {

class InterventionLibrary {
public:
    void add(const Intervention& intervention);
    void remove(const std::string& name);

    const Intervention* get(const std::string& name) const;
    std::vector<Intervention> all() const;

    std::vector<Intervention> active_at(const etwin::core::Instant& t) const;

private:
    std::vector<Intervention> interventions_;
};

} // namespace etwin::scenario
