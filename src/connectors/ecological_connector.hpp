#pragma once

#include "catalog/connector.hpp"

namespace etwin::connectors {

class EcologicalConnector : public etwin::catalog::DataConnector {
public:
    std::string name() const override { return "ecological"; }
    std::string description() const override;
    std::string source_uri() const override { return "local://ecological/occurrences"; }

    std::vector<etwin::catalog::DatasetDescriptor> discover() override;
    etwin::catalog::DataChunk fetch(const etwin::catalog::Query& query) override;
    etwin::core::quality::ValidationReport validate(
        const etwin::catalog::DataChunk& chunk) override;
};

} // namespace etwin::connectors
