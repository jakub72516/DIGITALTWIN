#pragma once

#include <vector>
#include <memory>

#include "core/time.hpp"
#include "state/earth_state.hpp"

namespace etwin::coupling {

class TimeInterpolationService {
public:
    etwin::state::Field interpolate(const etwin::state::Field& before,
                                    const etwin::state::Field& after,
                                    const etwin::core::Instant& t) const;

    static etwin::state::Field linear(const etwin::state::Field& before,
                                      const etwin::state::Field& after,
                                      const etwin::core::Instant& t);

    static etwin::state::Field nearest(const etwin::state::Field& before,
                                       const etwin::state::Field& after,
                                       const etwin::core::Instant& t);

    double interpolation_error(const etwin::state::Field& estimated,
                               const etwin::state::Field& reference) const;
};

} // namespace etwin::coupling
