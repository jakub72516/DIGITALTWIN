#include "coupling/regridding.hpp"

#include <cmath>

namespace etwin::coupling {

etwin::state::Field RegriddingService::regrid(
    const etwin::state::Field& source,
    const etwin::core::GridDefinition& target_grid,
    etwin::core::spatial::RegridMethod method) const {
    switch (method) {
        case etwin::core::spatial::RegridMethod::NearestNeighbor:
            return nearest_neighbor(source, target_grid);
        case etwin::core::spatial::RegridMethod::Bilinear:
            return bilinear(source, target_grid);
        case etwin::core::spatial::RegridMethod::AreaWeighted:
        case etwin::core::spatial::RegridMethod::Conservative:
            return area_weighted(source, target_grid);
    }
    return source;
}

etwin::state::Field RegriddingService::nearest_neighbor(
    const etwin::state::Field& source,
    const etwin::core::GridDefinition& target) {
    etwin::state::Field result = source;
    result.metadata.shape = {static_cast<size_t>(target.size())};
    result.data.assign(target.size(), 0.0);
    return result;
}

etwin::state::Field RegriddingService::bilinear(
    const etwin::state::Field& source,
    const etwin::core::GridDefinition& target) {
    etwin::state::Field result = source;
    result.metadata.shape = {static_cast<size_t>(target.size())};
    result.data.assign(target.size(), 0.0);
    return result;
}

etwin::state::Field RegriddingService::area_weighted(
    const etwin::state::Field& source,
    const etwin::core::GridDefinition& target) {
    etwin::state::Field result = source;
    result.metadata.shape = {static_cast<size_t>(target.size())};
    result.data.assign(target.size(), 0.0);
    return result;
}

double RegriddingService::mass_conservation_error(
    const etwin::state::Field& source,
    const etwin::state::Field& target) const {
    if (source.data.empty() && target.data.empty()) return 0.0;
    double src_sum = 0.0;
    double tgt_sum = 0.0;
    for (double v : source.data) src_sum += v;
    for (double v : target.data) tgt_sum += v;
    if (src_sum == 0.0) return 0.0;
    return std::abs(tgt_sum - src_sum) / std::abs(src_sum);
}

} // namespace etwin::coupling
