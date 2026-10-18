#pragma once

#include "catalog/connector.hpp"

namespace etwin::connectors {

class InfrastructureConnector : public etwin::catalog::DataConnector {
public:
    std::string name() const override { return "infrastructure"; }
    std::string description() const override;
    std::string source_uri() const override { return "local://infrastructure/osm"; }

    std::vector<etwin::catalog::DatasetDescriptor> discover() override;
    etwin::catalog::DataChunk fetch(const etwin::catalog::Query& query) override;
    etwin::core::quality::ValidationReport validate(
        const etwin::catalog::DataChunk& chunk) override;
};

} // namespace etwin::connectors
