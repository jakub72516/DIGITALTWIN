#include "coupling/coupling_service.hpp"

#include <stdexcept>
#include <cmath>
#include <algorithm>

namespace etwin::coupling {

void CouplingService::add_link(const CouplingLink& link) {
    links_.push_back(link);
}

void CouplingService::remove_link(const std::string& source_model, const std::string& target_model) {
    links_.erase(
        std::remove_if(links_.begin(), links_.end(),
            [&](const CouplingLink& l) {
                return l.source_model == source_model && l.target_model == target_model;
            }),
        links_.end());
}

CouplingCheckResult CouplingService::check_units(const CouplingLink& link,
                                                 const etwin::core::Unit& source_unit,
                                                 const etwin::core::Unit& target_unit) const {
    CouplingCheckResult result;
    result.passed = source_unit.compatible(target_unit);
    if (!result.passed) {
        result.issues.push_back(
            "Unit mismatch on link " + link.source_model + "." + link.source_field +
            " -> " + link.target_model + "." + link.target_field +
            ": " + source_unit.symbol + " vs " + target_unit.symbol);
    }
    return result;
}

CouplingCheckResult CouplingService::check_timestep(const CouplingLink& link,
                                                    etwin::core::Nanoseconds source_dt,
                                                    etwin::core::Nanoseconds target_dt) const {
    CouplingCheckResult result;
    result.passed = true;
    if (source_dt.count() <= 0 || target_dt.count() <= 0) {
        result.issues.push_back("Non-positive timestep on link " + link.source_model + " -> " + link.target_model);
        result.passed = false;
    } else if (source_dt.count() % target_dt.count() != 0 && target_dt.count() % source_dt.count() != 0) {
        result.warnings.push_back("Non-divisible timesteps on link " + link.source_model + " -> " + link.target_model);
    }
    return result;
}

CouplingCheckResult CouplingService::check_resolution(const CouplingLink& link,
                                                      double source_resolution,
                                                      double target_resolution) const {
    CouplingCheckResult result;
    result.passed = true;
    if (source_resolution > 0.0 && target_resolution > 0.0 &&
        source_resolution < target_resolution * 0.5) {
        result.warnings.push_back("Source resolution finer than target on link " + link.source_model + " -> " + link.target_model);
    }
    return result;
}

etwin::state::Field CouplingService::exchange(const CouplingLink& link,
                                              const etwin::state::Field& source_field,
                                              const etwin::models::ExecutionContext& ctx) {
    etwin::state::Field result = source_field;
    result.metadata.name = link.target_field;

    ExchangeRecord record;
    record.source = link.source_model;
    record.target = link.target_model;
    record.field = link.source_field;
    record.time = ctx.current_time;
    record.unit_compatible = true;
    record.time_compatible = true;
    record.resolution_compatible = true;
    record.notes = "exchanged";
    record_exchange(record);

    return result;
}

void CouplingService::record_exchange(const ExchangeRecord& record) {
    history_.push_back(record);
}

void CouplingService::validate_all() const {
    for (const auto& link : links_) {
        if (link.source_model.empty() || link.target_model.empty() ||
            link.source_field.empty() || link.target_field.empty()) {
            throw std::runtime_error("Invalid coupling link (empty model or field)");
        }
    }
}

} // namespace etwin::coupling
