#pragma once

#include <cstdint>
#include <string>
#include <chrono>
#include <stdexcept>
#include <format>
#include <compare>

namespace etwin::core {

using Nanoseconds = std::chrono::nanoseconds;
using Seconds = std::chrono::seconds;
using Hours = std::chrono::hours;
using Days = std::chrono::duration<int64_t, std::ratio<86400>>;

class Instant {
public:
    constexpr Instant() = default;
    constexpr explicit Instant(int64_t nanoseconds_since_epoch)
        : ns_since_epoch_(nanoseconds_since_epoch) {}

    static Instant from_unix_seconds(double seconds) {
        return Instant(static_cast<int64_t>(seconds * 1e9));
    }

    static Instant from_unix_milliseconds(int64_t millis) {
        return Instant(millis * 1'000'000);
    }

    static Instant now() {
        auto now = std::chrono::system_clock::now();
        auto ns = std::chrono::duration_cast<Nanoseconds>(now.time_since_epoch());
        return Instant(ns.count());
    }

    static Instant from_iso8601(const std::string& iso);

    constexpr int64_t nanoseconds_since_epoch() const { return ns_since_epoch_; }
    constexpr double unix_seconds() const {
        return static_cast<double>(ns_since_epoch_) / 1e9;
    }

    std::string to_iso8601() const;

    constexpr std::strong_ordering operator<=>(const Instant&) const = default;
    constexpr bool operator==(const Instant&) const = default;

    constexpr Instant operator+(Nanoseconds d) const {
        return Instant(ns_since_epoch_ + d.count());
    }

    constexpr Instant operator-(Nanoseconds d) const {
        return Instant(ns_since_epoch_ - d.count());
    }

    constexpr Nanoseconds operator-(const Instant& other) const {
        return Nanoseconds(ns_since_epoch_ - other.ns_since_epoch_);
    }

private:
    int64_t ns_since_epoch_ = 0;
};

enum class CalendarType {
    Gregorian,
    NoLeap,
    Day360,
    Julian,
};

class TimeRange {
public:
    constexpr TimeRange() = default;

    constexpr TimeRange(Instant start, Instant end) : start_(start), end_(end) {
        if (end < start) throw std::invalid_argument("TimeRange end before start");
    }

    constexpr Instant start() const { return start_; }
    constexpr Instant end() const { return end_; }
    constexpr Nanoseconds duration() const { return end_ - start_; }
    constexpr bool contains(Instant t) const { return t >= start_ && t < end_; }

private:
    Instant start_;
    Instant end_;
};

class TimeStep {
public:
    enum class Type { Fixed, Adaptive, Irregular };

    static TimeStep fixed(Nanoseconds dt) { return TimeStep(Type::Fixed, dt.count(), {}); }
    static TimeStep irregular(std::vector<int64_t> offsets) {
        return TimeStep(Type::Irregular, 0, std::move(offsets));
    }

    Type type() const { return type_; }
    Nanoseconds fixed_dt() const;
    const std::vector<int64_t>& irregular_offsets() const;

private:
    TimeStep(Type type, int64_t dt, std::vector<int64_t> offsets)
        : type_(type), fixed_ns_(dt), irregular_ns_(std::move(offsets)) {}

    Type type_;
    int64_t fixed_ns_;
    std::vector<int64_t> irregular_ns_;
};

class TimeScale {
public:
    virtual ~TimeScale() = default;
    virtual Instant to_utc(const Instant& t) const = 0;
    virtual Instant from_utc(const Instant& utc) const = 0;
    virtual std::string name() const = 0;
};

class UTCTimeScale : public TimeScale {
public:
    Instant to_utc(const Instant& t) const override { return t; }
    Instant from_utc(const Instant& utc) const override { return utc; }
    std::string name() const override { return "UTC"; }
};

} // namespace etwin::core
