#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <functional>

#include "core/time.hpp"
#include "core/units.hpp"
#include "core/quality.hpp"

#include "models/model_module.hpp"
#include "state/earth_state.hpp"

namespace etwin::coupling {

enum class CouplingMode {
    OneWay,
    Iterative,
    LooselySynchronized,
};

struct CouplingLink {
    std::string source_model;
    std::string source_field;
    std::string target_model;
    std::string target_field;
    CouplingMode mode;
};

struct CouplingCheckResult {
    bool passed;
    std::vector<std::string> issues;
    std::vector<std::string> warnings;
};

struct ExchangeRecord {
    std::string source;
    std::string target;
    std::string field;
    etwin::core::Instant time;
    bool unit_compatible;
    bool time_compatible;
    bool resolution_compatible;
    std::string notes;
};

class CouplingService {
public:
    void add_link(const CouplingLink& link);
    void remove_link(const std::string& source_model, const std::string& target_model);

    CouplingCheckResult check_units(const CouplingLink& link,
                                    const etwin::core::Unit& source_unit,
                                    const etwin::core::Unit& target_unit) const;

    CouplingCheckResult check_timestep(const CouplingLink& link,
                                       etwin::core::Nanoseconds source_dt,
                                       etwin::core::Nanoseconds target_dt) const;

    CouplingCheckResult check_resolution(const CouplingLink& link,
                                         double source_resolution,
                                         double target_resolution) const;

    etwin::state::Field exchange(const CouplingLink& link,
                                 const etwin::state::Field& source_field,
                                 const etwin::models::ExecutionContext& ctx);

    void record_exchange(const ExchangeRecord& record);
    const std::vector<ExchangeRecord>& history() const { return history_; }

    std::vector<CouplingLink> links() const { return links_; }
    void validate_all() const;

private:
    std::vector<CouplingLink> links_;
    std::vector<ExchangeRecord> history_;
};

} // namespace etwin::coupling
