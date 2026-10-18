#pragma once

#include <string>
#include <stdexcept>
#include <cmath>
#include <cstdint>
#include <format>
#include <numbers>

namespace etwin::core {

enum class Dimension : int {
    Length = 0,
    Mass = 1,
    Time = 2,
    Temperature = 3,
    Amount = 4,
    Current = 5,
    Luminosity = 6,
};

template <Dimension D, int Power>
struct DimensionExponent {
    static constexpr Dimension dim = D;
    static constexpr int power = Power;
};

struct UnitDimension {
    int length = 0;
    int mass = 0;
    int time = 0;
    int temperature = 0;
    int amount = 0;
    int current = 0;
    int luminosity = 0;

    constexpr bool operator==(const UnitDimension&) const = default;
};

constexpr UnitDimension operator*(const UnitDimension& a, const UnitDimension& b) {
    return UnitDimension{
        a.length + b.length,
        a.mass + b.mass,
        a.time + b.time,
        a.temperature + b.temperature,
        a.amount + b.amount,
        a.current + b.current,
        a.luminosity + b.luminosity,
    };
}

constexpr UnitDimension operator/(const UnitDimension& a, const UnitDimension& b) {
    return UnitDimension{
        a.length - b.length,
        a.mass - b.mass,
        a.time - b.time,
        a.temperature - b.temperature,
        a.amount - b.amount,
        a.current - b.current,
        a.luminosity - b.luminosity,
    };
}

constexpr UnitDimension pow(const UnitDimension& d, int p) {
    return UnitDimension{
        d.length * p,
        d.mass * p,
        d.time * p,
        d.temperature * p,
        d.amount * p,
        d.current * p,
        d.luminosity * p,
    };
}

struct Unit {
    std::string symbol;
    UnitDimension dim;
    double to_si_factor;
    double offset;

    constexpr double to_si(double value) const {
        return value * to_si_factor + offset;
    }

    constexpr double from_si(double si_value) const {
        return (si_value - offset) / to_si_factor;
    }

    constexpr bool compatible(const Unit& other) const {
        return dim == other.dim;
    }

    constexpr Unit operator*(const Unit& other) const {
        return Unit{
            symbol + "*" + other.symbol,
            dim * other.dim,
            to_si_factor * other.to_si_factor,
            0.0,
        };
    }

    constexpr Unit operator/(const Unit& other) const {
        return Unit{
            symbol + "/" + other.symbol,
            dim / other.dim,
            to_si_factor / other.to_si_factor,
            0.0,
        };
    }
};

namespace units {
    constexpr Unit meter{"m", UnitDimension{1}, 1.0, 0.0};
    constexpr Unit kilometer{"km", UnitDimension{1}, 1000.0, 0.0};
    constexpr Unit kilogram{"kg", UnitDimension{0,1}, 1.0, 0.0};
    constexpr Unit gram{"g", UnitDimension{0,1}, 0.001, 0.0};
    constexpr Unit second{"s", UnitDimension{0,0,1}, 1.0, 0.0};
    constexpr Unit hour{"h", UnitDimension{0,0,1}, 3600.0, 0.0};
    constexpr Unit day{"d", UnitDimension{0,0,1}, 86400.0, 0.0};
    constexpr Unit kelvin{"K", UnitDimension{0,0,0,1}, 1.0, 0.0};
    constexpr Unit celsius{"degC", UnitDimension{0,0,0,1}, 1.0, 273.15};
    constexpr Unit meter_per_second{"m/s", UnitDimension{1,0,-1}, 1.0, 0.0};
    constexpr Unit pascal{"Pa", UnitDimension{-1,1,-2}, 1.0, 0.0};
    constexpr Unit hectopascal{"hPa", UnitDimension{-1,1,-2}, 100.0, 0.0};
    constexpr Unit kilogram_per_m2{"kg/m^2", UnitDimension{-2,1}, 1.0, 0.0};
    constexpr Unit kilogram_per_m2_s{"kg/(m^2*s)", UnitDimension{-2,1,-1}, 1.0, 0.0};
    constexpr Unit meter_per_second_sq{"m/s^2", UnitDimension{1,0,-2}, 1.0, 0.0};
    constexpr Unit watt_per_m2{"W/m^2", UnitDimension{0,1,-3}, 1.0, 0.0};
    constexpr Unit joule_per_kg{"J/kg", UnitDimension{2,0,-2}, 1.0, 0.0};
    constexpr Unit dimensionless{"", UnitDimension{}, 1.0, 0.0};
    constexpr Unit percent{"%", UnitDimension{}, 0.01, 0.0};
    constexpr Unit degree{"deg", UnitDimension{}, std::numbers::pi / 180.0, 0.0};
}

class UnitMismatchError : public std::runtime_error {
public:
    explicit UnitMismatchError(const std::string& msg) : std::runtime_error(msg) {}
};

template <typename T = double>
class Quantity {
public:
    constexpr Quantity() = default;
    constexpr Quantity(T value, Unit unit) : value_(value), unit_(unit) {}

    constexpr T value() const { return value_; }
    constexpr const Unit& unit() const { return unit_; }

    constexpr auto in(const Unit& target) const {
        if (!unit_.compatible(target)) {
            throw UnitMismatchError(
                std::format("Cannot convert from {} to {}", unit_.symbol, target.symbol));
        }
        double si = unit_.to_si(value_);
        return Quantity<T>(target.from_si(si), target);
    }

    constexpr T si() const { return unit_.to_si(value_); }

    constexpr Quantity operator+(const Quantity& other) const {
        if (!unit_.compatible(other.unit_)) {
            throw UnitMismatchError("Incompatible units in addition");
        }
        return Quantity(value_ + other.in(unit_).value(), unit_);
    }

    constexpr Quantity operator-(const Quantity& other) const {
        if (!unit_.compatible(other.unit_)) {
            throw UnitMismatchError("Incompatible units in subtraction");
        }
        return Quantity(value_ - other.in(unit_).value(), unit_);
    }

    template <typename U>
    constexpr auto operator*(const Quantity<U>& other) const {
        return Quantity(value_ * other.value_, unit_ * other.unit_);
    }

    template <typename U>
    constexpr auto operator/(const Quantity<U>& other) const {
        return Quantity(value_ / other.value_, unit_ / other.unit_);
    }

    constexpr Quantity operator*(T scalar) const {
        return Quantity(value_ * scalar, unit_);
    }

    constexpr Quantity operator/(T scalar) const {
        return Quantity(value_ / scalar, unit_);
    }

    constexpr bool operator==(const Quantity&) const = default;

private:
    T value_{};
    Unit unit_ = units::dimensionless;
};

template <typename T, typename U>
constexpr auto operator*(T scalar, const Quantity<U>& q) {
    return q * scalar;
}

using QuantityF = Quantity<float>;
using QuantityD = Quantity<double>;

} // namespace etwin::core
