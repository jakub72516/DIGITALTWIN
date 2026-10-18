#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <optional>
#include <functional>

#include "core/time.hpp"
#include "core/units.hpp"
#include "core/crs.hpp"
#include "core/provenance.hpp"
#include "core/uncertainty.hpp"

#include "state/earth_state.hpp"
#include "state/query.hpp"

namespace etwin::models {

struct FieldRequirement {
    std::string name;
    etwin::core::Unit unit;
    std::string description;
    bool required;
    double max_tolerable_resolution_m;
    etwin::core::Nanoseconds max_tolerable_time_lag;
};

struct FieldProduct {
    std::string name;
    etwin::core::Unit unit;
    etwin::state::FieldKind kind;
    std::string description;
};

struct ModelMetadata {
    std::string name;
    std::string version;
    std::string description;
    std::vector<std::string> equations_or_methods;
    std::vector<std::string> limitations;
    std::vector<std::string> valid_conditions;
    std::vector<std::string> known_failure_modes;
    std::string calibration_procedure;
    std::vector<std::string> dependencies;
};

struct ExecutionContext {
    etwin::core::Instant current_time;
    etwin::core::Nanoseconds time_step;
    uint64_t step_number;
    uint64_t random_seed;
    std::map<std::string, double> parameters;
};

class ModelModule {
public:
    virtual ~ModelModule() = default;

    virtual ModelMetadata metadata() const = 0;

    virtual std::vector<FieldRequirement> requirements() const = 0;
    virtual std::vector<FieldProduct> products() const = 0;

    virtual void initialize(const ExecutionContext& ctx) = 0;
    virtual void step(const ExecutionContext& ctx) = 0;
    virtual void finalize(const ExecutionContext& ctx) = 0;

    virtual void validate_inputs(const etwin::state::EarthStateQuery& query) const = 0;
    virtual void calibrate(const std::map<std::string, std::vector<double>>& calibration_data) = 0;

    virtual std::map<std::string, etwin::state::Field> output() const = 0;

    virtual bool is_converged() const { return true; }
    virtual void reset() = 0;
};

class ModelFactory {
public:
    using Creator = std::function<std::unique_ptr<ModelModule>()>;

    static ModelFactory& instance();

    void register_model(const std::string& name, Creator creator);
    std::unique_ptr<ModelModule> create(const std::string& name) const;
    std::vector<std::string> available_models() const;

private:
    std::map<std::string, Creator> creators_;
};

} // namespace etwin::models
