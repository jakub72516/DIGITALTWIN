#pragma once

#include <vector>
#include <string>
#include <memory>
#include <functional>

#include "core/spatial.hpp"
#include "state/earth_state.hpp"

namespace etwin::coupling {

class RegriddingService {
public:
    etwin::state::Field regrid(const etwin::state::Field& source,
                               const etwin::core::GridDefinition& target_grid,
                               etwin::core::spatial::RegridMethod method) const;

    static etwin::state::Field nearest_neighbor(const etwin::state::Field& source,
                                                const etwin::core::GridDefinition& target);

    static etwin::state::Field bilinear(const etwin::state::Field& source,
                                        const etwin::core::GridDefinition& target);

    static etwin::state::Field area_weighted(const etwin::state::Field& source,
                                             const etwin::core::GridDefinition& target);

    double mass_conservation_error(const etwin::state::Field& source,
                                   const etwin::state::Field& target) const;
};

} // namespace etwin::coupling
