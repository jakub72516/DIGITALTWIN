#include "catalog/catalog.hpp"

#include <nlohmann/json.hpp>
#include <fstream>
#include <algorithm>
#include <stdexcept>

namespace etwin::catalog {

bool DatasetDescriptor::is_current() const {
    return temporal.end > etwin::core::Instant::now();
}

std::string DatasetDescriptor::to_json() const {
    nlohmann::json j;
    j["id"] = id;
    j["name"] = name;
    j["description"] = description;
    j["source_url"] = source_url;
    j["version"] = version;
    j["organization"] = organization;
    j["license"] = license.name;
    j["license_type"] = static_cast<int>(license.type);
    j["min_lat"] = spatial.region.min_lat;
    j["max_lat"] = spatial.region.max_lat;
    j["min_lon"] = spatial.region.min_lon;
    j["max_lon"] = spatial.region.max_lon;
    j["global"] = spatial.global;
    j["start"] = temporal.start.to_iso8601();
    j["end"] = temporal.end.to_iso8601();
    j["spatial_resolution_m"] = spatial_resolution_m;
    j["variables"] = variables;
    j["update_frequency"] = update_frequency;
    j["quality_status"] = quality_status;
    j["lineage"] = lineage;
    j["tags"] = tags;
    return j.dump();
}

DatasetDescriptor DatasetDescriptor::from_json(const std::string& json) {
    nlohmann::json j = nlohmann::json::parse(json);
    DatasetDescriptor d;
    d.id = j.value("id", "");
    d.name = j.value("name", "");
    d.description = j.value("description", "");
    d.source_url = j.value("source_url", "");
    d.version = j.value("version", "");
    d.organization = j.value("organization", "");
    d.license.name = j.value("license", "");
    d.license.type = static_cast<LicenseType>(j.value("license_type", 4));
    d.spatial.region.min_lat = j.value("min_lat", -90.0);
    d.spatial.region.max_lat = j.value("max_lat", 90.0);
    d.spatial.region.min_lon = j.value("min_lon", -180.0);
    d.spatial.region.max_lon = j.value("max_lon", 180.0);
    d.spatial.global = j.value("global", false);
    d.temporal.start = etwin::core::Instant::from_iso8601(j.value("start", "1970-01-01T00:00:00Z"));
    d.temporal.end = etwin::core::Instant::from_iso8601(j.value("end", "2100-01-01T00:00:00Z"));
    d.spatial_resolution_m = j.value("spatial_resolution_m", 0.0);
    d.variables = j.value("variables", std::vector<std::string>{});
    d.update_frequency = j.value("update_frequency", "");
    d.quality_status = j.value("quality_status", "");
    d.lineage = j.value("lineage", "");
    d.tags = j.value("tags", std::vector<std::string>{});
    return d;
}

Catalog::Catalog() = default;

void Catalog::register_dataset(const DatasetDescriptor& desc) {
    if (desc.id.empty()) {
        throw std::invalid_argument("Dataset requires non-empty id");
    }
    if (datasets_.contains(desc.id)) {
        throw std::invalid_argument("Dataset already registered: " + desc.id);
    }
    datasets_[desc.id] = desc;
}

void Catalog::remove_dataset(const std::string& id) {
    datasets_.erase(id);
}

void Catalog::update_dataset(const DatasetDescriptor& desc) {
    datasets_[desc.id] = desc;
}

std::optional<DatasetDescriptor> Catalog::lookup(const std::string& id) const {
    auto it = datasets_.find(id);
    if (it == datasets_.end()) return std::nullopt;
    return it->second;
}

std::vector<DatasetDescriptor> Catalog::list_all() const {
    std::vector<DatasetDescriptor> result;
    result.reserve(datasets_.size());
    for (const auto& [id, d] : datasets_) result.push_back(d);
    return result;
}

std::vector<DatasetDescriptor> Catalog::query_by_region(const etwin::core::GeoRegion& region) const {
    std::vector<DatasetDescriptor> result;
    for (const auto& [id, d] : datasets_) {
        if (d.spatial.global) {
            result.push_back(d);
            continue;
        }
        etwin::core::GeoRegion r = d.spatial.region;
        bool overlaps = !(r.max_lat < region.min_lat || r.min_lat > region.max_lat ||
                          r.max_lon < region.min_lon || r.min_lon > region.max_lon);
        if (overlaps) result.push_back(d);
    }
    return result;
}

std::vector<DatasetDescriptor> Catalog::query_by_time(const etwin::core::Instant& t) const {
    std::vector<DatasetDescriptor> result;
    for (const auto& [id, d] : datasets_) {
        if (t >= d.temporal.start && t <= d.temporal.end) result.push_back(d);
    }
    return result;
}

std::vector<DatasetDescriptor> Catalog::query_by_variable(const std::string& variable) const {
    std::vector<DatasetDescriptor> result;
    for (const auto& [id, d] : datasets_) {
        if (std::find(d.variables.begin(), d.variables.end(), variable) != d.variables.end()) {
            result.push_back(d);
        }
    }
    return result;
}

std::vector<DatasetDescriptor> Catalog::query_by_tag(const std::string& tag) const {
    std::vector<DatasetDescriptor> result;
    for (const auto& [id, d] : datasets_) {
        if (std::find(d.tags.begin(), d.tags.end(), tag) != d.tags.end()) {
            result.push_back(d);
        }
    }
    return result;
}

LicenseCompatibility Catalog::check_license(const std::string& id, LicenseType intended_use) const {
    auto it = datasets_.find(id);
    if (it == datasets_.end()) return LicenseCompatibility::Incompatible;
    const auto& lic = it->second.license;
    switch (lic.type) {
        case LicenseType::PublicDomain:
            return LicenseCompatibility::Compatible;
        case LicenseType::Open:
            return LicenseCompatibility::Compatible;
        case LicenseType::Restricted:
            return lic.allows_redistribution ? LicenseCompatibility::RequiresAttribution
                                             : LicenseCompatibility::Incompatible;
        case LicenseType::Proprietary:
            return LicenseCompatibility::Incompatible;
        default:
            return LicenseCompatibility::RequiresAttribution;
    }
}

bool Catalog::save(const std::string& path) const {
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& [id, d] : datasets_) {
        arr.push_back(nlohmann::json::parse(d.to_json()));
    }
    std::ofstream f(path);
    if (!f) return false;
    f << arr.dump(2);
    return true;
}

bool Catalog::load(const std::string& path) {
    std::ifstream f(path);
    if (!f) return false;
    nlohmann::json arr;
    f >> arr;
    datasets_.clear();
    for (const auto& j : arr) {
        DatasetDescriptor d = DatasetDescriptor::from_json(j.dump());
        datasets_[d.id] = d;
    }
    return true;
}

} // namespace etwin::catalog
