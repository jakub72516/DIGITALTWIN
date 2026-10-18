#include "catalog/connector.hpp"

#include <stdexcept>

namespace etwin::catalog {

void ConnectorRegistry::register_connector(std::shared_ptr<DataConnector> connector) {
    if (!connector) throw std::invalid_argument("Null connector");
    connectors_[connector->name()] = connector;
}

void ConnectorRegistry::unregister_connector(const std::string& name) {
    connectors_.erase(name);
}

std::shared_ptr<DataConnector> ConnectorRegistry::get(const std::string& name) const {
    auto it = connectors_.find(name);
    if (it == connectors_.end()) return nullptr;
    return it->second;
}

std::vector<std::shared_ptr<DataConnector>> ConnectorRegistry::all() const {
    std::vector<std::shared_ptr<DataConnector>> result;
    result.reserve(connectors_.size());
    for (const auto& [name, c] : connectors_) result.push_back(c);
    return result;
}

std::vector<std::string> ConnectorRegistry::names() const {
    std::vector<std::string> result;
    for (const auto& [name, c] : connectors_) result.push_back(name);
    return result;
}

std::vector<DatasetDescriptor> ConnectorRegistry::discover_all() const {
    std::vector<DatasetDescriptor> result;
    for (const auto& [name, c] : connectors_) {
        auto descs = c->discover();
        result.insert(result.end(), descs.begin(), descs.end());
    }
    return result;
}

void DataAvailability::record(const Source& s) {
    sources_.push_back(s);
}

std::vector<DataAvailability::Source> DataAvailability::all() const {
    return sources_;
}

std::vector<DataAvailability::Source> DataAvailability::unavailable() const {
    std::vector<Source> result;
    for (const auto& s : sources_) {
        if (!s.available) result.push_back(s);
    }
    return result;
}

} // namespace etwin::catalog
