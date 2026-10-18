#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <functional>
#include <format>
#include <map>
#include <optional>

#include "time.hpp"

namespace etwin::core {

using Hash = std::string;

std::string sha256(const std::string& data);
std::string sha256_file(const std::string& path);
Hash hash_combine(const Hash& a, const Hash& b);

struct DataSource {
    std::string dataset_id;
    std::string version;
    std::string source_url;
    std::string license;
    std::optional<Instant> retrieval_time;
    std::optional<Hash> content_hash;
};

struct TransformRecord {
    std::string algorithm;
    std::string version;
    std::map<std::string, std::string> parameters;
    Hash input_hash;
    Hash output_hash;
    std::string description;
};

struct QualityFlags {
    bool validated = false;
    bool gap_filled = false;
    bool interpolated = false;
    bool extrapolated = false;
    bool suspicious = false;
    bool rejected = false;
    double confidence = 1.0;
    std::string notes;

    bool usable() const { return validated && !rejected && !suspicious && confidence > 0.0; }
};

class Provenance {
public:
    Provenance() = default;
    explicit Provenance(DataSource source);

    void add_transform(const TransformRecord& transform);
    void set_quality(const QualityFlags& quality) { quality_ = quality; }

    const DataSource& source() const { return source_; }
    const std::vector<TransformRecord>& transforms() const { return transforms_; }
    const QualityFlags& quality() const { return quality_; }

    Hash compute_hash() const;
    std::string to_json() const;

    static Provenance from_json(const std::string& json);

    bool is_reproducible() const;
    bool has_known_issues() const;

private:
    DataSource source_;
    std::vector<TransformRecord> transforms_;
    QualityFlags quality_;
};

} // namespace etwin::core
