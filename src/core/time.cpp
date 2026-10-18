#include "core/time.hpp"
#include <sstream>
#include <iomanip>

namespace etwin::core {

Instant Instant::from_iso8601(const std::string& iso) {
    std::tm tm = {};
    std::istringstream ss(iso);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    if (ss.fail()) {
        throw std::invalid_argument(std::format("Invalid ISO 8601: {}", iso));
    }
    auto time = std::mktime(&tm);
    return Instant::from_unix_seconds(static_cast<double>(time));
}

std::string Instant::to_iso8601() const {
    time_t sec = static_cast<time_t>(ns_since_epoch_ / 1'000'000'000);
    std::tm tm;
#ifdef _WIN32
    gmtime_s(&tm, &sec);
#else
    gmtime_r(&sec, &tm);
#endif
    std::ostringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}

Nanoseconds TimeStep::fixed_dt() const {
    if (type_ != Type::Fixed) throw std::runtime_error("Not a fixed timestep");
    return Nanoseconds(fixed_ns_);
}

const std::vector<int64_t>& TimeStep::irregular_offsets() const {
    if (type_ != Type::Irregular) throw std::runtime_error("Not irregular timestep");
    return irregular_ns_;
}

} // namespace etwin::core
