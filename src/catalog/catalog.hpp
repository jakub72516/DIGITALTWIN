#pragma once

#include <string>
#include <vector>
#include <optional>
#include <memory>
#include <map>

#include "core/time.hpp"
#include "core/crs.hpp"
#include "core/provenance.hpp"

namespace etwin::catalog {

enum class LicenseType {
    PublicDomain,
    Open,
    Restricted,
    Proprietary,
    Unknown,
};

struct License {
    LicenseType type;
    std::string name;
    std::string url;
    std::string attribution;
    bool allows_redistribution;
    bool allows_commercial_use;
    bool allows_modification;
};

struct SpatialCoverage {
    etwin::core::GeoRegion region;
    bool global;
};

struct TemporalCoverage {
    etwin::core::Instant start;
    etwin::core::Instant end;
    std::optional<etwin::core::Nanoseconds> resolution;
};

struct DatasetDescriptor {
    std::string id;
    std::string name;
    std::string description;
    std::string source_url;
    std::string version;
    std::string organization;

    License license;

    SpatialCoverage spatial;
    TemporalCoverage temporal;

    double spatial_resolution_m;
    std::vector<std::string> variables;

    std::string update_frequency;
    std::string quality_status;
    std::string lineage;

    std::vector<std::string> tags;

    bool is_current() const;
    std::string to_json() const;
    static DatasetDescriptor from_json(const std::string& json);
};

enum class LicenseCompatibility {
    Compatible,
    RequiresAttribution,
    Incompatible,
};

class Catalog {
public:
    Catalog();

    void register_dataset(const DatasetDescriptor& desc);
    void remove_dataset(const std::string& id);
    void update_dataset(const DatasetDescriptor& desc);

    std::optional<DatasetDescriptor> lookup(const std::string& id) const;
    std::vector<DatasetDescriptor> list_all() const;
    std::vector<DatasetDescriptor> query_by_region(const etwin::core::GeoRegion& region) const;
    std::vector<DatasetDescriptor> query_by_time(const etwin::core::Instant& t) const;
    std::vector<DatasetDescriptor> query_by_variable(const std::string& variable) const;
    std::vector<DatasetDescriptor> query_by_tag(const std::string& tag) const;

    LicenseCompatibility check_license(const std::string& id, LicenseType intended_use) const;

    bool save(const std::string& path) const;
    bool load(const std::string& path);

    size_t size() const { return datasets_.size(); }

private:
    std::map<std::string, DatasetDescriptor> datasets_;
};

} // namespace etwin::catalog
