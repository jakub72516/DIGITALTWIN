#include "coupling/time_interpolation.hpp"

#include <cmath>
#include <stdexcept>
#include <algorithm>

namespace etwin::coupling {

etwin::state::Field TimeInterpolationService::interpolate(
    const etwin::state::Field& before,
    const etwin::state::Field& after,
    const etwin::core::Instant& t) const {
    if (before.timestamp == after.timestamp) return before;
    if (t < before.timestamp || t > after.timestamp) {
        throw std::invalid_argument("Interpolation time outside [before, after]");
    }
    return linear(before, after, t);
}

etwin::state::Field TimeInterpolationService::linear(
    const etwin::state::Field& before,
    const etwin::state::Field& after,
    const etwin::core::Instant& t) {
    etwin::state::Field result = before;
    if (before.data.empty() || after.data.empty()) return before;

    double dt_total = static_cast<double>((after.timestamp - before.timestamp).count());
    double dt = static_cast<double>((t - before.timestamp).count());

    if (dt_total <= 0.0) return before;
    double alpha = dt / dt_total;
    alpha = std::clamp(alpha, 0.0, 1.0);

    size_t n = std::min(before.data.size(), after.data.size());
    result.data.resize(n);
    for (size_t i = 0; i < n; ++i) {
        result.data[i] = before.data[i] + alpha * (after.data[i] - before.data[i]);
    }
    result.timestamp = t;

    if (result.quality.confidence > 0.0) {
        result.quality.confidence = result.quality.confidence * (1.0 - 0.1 * alpha);
        result.quality.interpolated = true;
    }
    return result;
}

etwin::state::Field TimeInterpolationService::nearest(
    const etwin::state::Field& before,
    const etwin::state::Field& after,
    const etwin::core::Instant& t) {
    auto dt_before = (t - before.timestamp).count();
    auto dt_after = (after.timestamp - t).count();
    etwin::state::Field result = (dt_before <= dt_after) ? before : after;
    result.timestamp = t;
    result.quality.interpolated = true;
    return result;
}

double TimeInterpolationService::interpolation_error(
    const etwin::state::Field& estimated,
    const etwin::state::Field& reference) const {
    if (estimated.data.empty() || reference.data.empty()) return 0.0;
    size_t n = std::min(estimated.data.size(), reference.data.size());
    double sum_sq = 0.0;
    for (size_t i = 0; i < n; ++i) {
        double diff = estimated.data[i] - reference.data[i];
        sum_sq += diff * diff;
    }
    return std::sqrt(sum_sq / n);
}

} // namespace etwin::coupling
