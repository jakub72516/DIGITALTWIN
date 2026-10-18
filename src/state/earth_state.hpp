#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <optional>
#include <functional>

#include "core/time.hpp"
#include "core/crs.hpp"
#include "core/provenance.hpp"
#include "core/units.hpp"
#include "core/spatial.hpp"
#include "core/uncertainty.hpp"

namespace etwin::state {

enum class FieldKind {
    Observation,
    Derived,
    Simulation,
};

enum class DataType {
    Scalar,
    Vector,
    Tensor,
};

struct FieldMetadata {
    std::string name;
    etwin::core::Unit unit;
    etwin::core::CRS crs;
    DataType type;
    FieldKind kind;
    double resolution_m;
    std::vector<size_t> shape;
};

struct Field {
    FieldMetadata metadata;
    std::vector<double> data;
    etwin::core::Provenance provenance;
    etwin::core::QualityFlags quality;
    etwin::core::uncertainty::Distribution uncertainty;
    etwin::core::Instant timestamp;
    std::optional<double> validity_score;

    bool is_observation() const {
        return metadata.kind == FieldKind::Observation;
    }
    bool is_derived() const {
        return metadata.kind == FieldKind::Derived;
    }
    bool is_simulation() const {
        return metadata.kind == FieldKind::Simulation;
    }

    std::string kind_string() const;
};

class EarthState {
public:
    EarthState();
    explicit EarthState(std::string version);

    void add_field(const Field& field);
    void remove_field(const std::string& name, const etwin::core::Instant& t);
    void update_field(const Field& field);

    std::optional<Field> get(const std::string& name, const etwin::core::Instant& t) const;
    std::vector<Field> all_fields() const;
    std::vector<std::string> field_names() const;
    std::vector<etwin::core::Instant> timestamps() const;

    const std::string& version() const { return version_; }

    bool empty() const { return fields_.empty(); }
    size_t size() const { return fields_.size(); }

    etwin::core::Hash compute_hash() const;

private:
    std::string version_;
    std::map<std::string, std::vector<Field>> fields_;
};

class StateHistory {
public:
    void checkpoint(const EarthState& state, const etwin::core::Instant& t);
    std::optional<EarthState> restore(const etwin::core::Instant& t) const;
    std::vector<etwin::core::Instant> checkpoint_times() const;
    void prune_before(const etwin::core::Instant& t);
    size_t size() const { return checkpoints_.size(); }

private:
    std::map<int64_t, EarthState> checkpoints_;
};

} // namespace etwin::state
