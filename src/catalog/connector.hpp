#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <functional>
#include <map>

#include "core/time.hpp"
#include "core/crs.hpp"
#include "core/provenance.hpp"
#include "core/units.hpp"
#include "core/quality.hpp"

#include "catalog/catalog.hpp"

namespace etwin::catalog {

struct Query {
    etwin::core::GeoRegion region;
    etwin::core::TimeRange time_range;
    std::vector<std::string> variables;
    std::optional<double> max_resolution_m;
};

struct DataChunk {
    std::string dataset_id;
    std::vector<double> data;
    std::vector<std::string> variable_names;
    std::vector<size_t> shape;
    etwin::core::CRS crs;
    etwin::core::Instant timestamp;
    etwin::core::Provenance provenance;
    etwin::core::quality::ValidationReport validation;

    bool empty() const { return data.empty(); }
};

class DataConnector {
public:
    virtual ~DataConnector() = default;

    virtual std::string name() const = 0;
    virtual std::string description() const = 0;

    virtual std::vector<DatasetDescriptor> discover() = 0;
    virtual DataChunk fetch(const Query& query) = 0;
    virtual etwin::core::quality::ValidationReport validate(const DataChunk& chunk) = 0;

    virtual bool is_available() const { return true; }
    virtual std::string source_uri() const = 0;
};

class ConnectorRegistry {
public:
    void register_connector(std::shared_ptr<DataConnector> connector);
    void unregister_connector(const std::string& name);

    std::shared_ptr<DataConnector> get(const std::string& name) const;
    std::vector<std::shared_ptr<DataConnector>> all() const;
    std::vector<std::string> names() const;

    std::vector<DatasetDescriptor> discover_all() const;

    size_t size() const { return connectors_.size(); }

private:
    std::map<std::string, std::shared_ptr<DataConnector>> connectors_;
};

class DataAvailability {
public:
    struct Source {
        std::string connector;
        std::string dataset_id;
        bool available;
        std::optional<etwin::core::Instant> last_success;
        std::optional<std::string> error;
    };

    void record(const Source& s);
    std::vector<Source> all() const;
    std::vector<Source> unavailable() const;

private:
    std::vector<Source> sources_;
};

} // namespace etwin::catalog
