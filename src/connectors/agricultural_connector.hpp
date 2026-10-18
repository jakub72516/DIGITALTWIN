#pragma once

#include "catalog/connector.hpp"

namespace etwin::connectors {

class AgriculturalConnector : public etwin::catalog::DataConnector {
public:
    std::string name() const override { return "agricultural"; }
    std::string description() const override;
    std::string source_uri() const override { return "local://agriculture/statistics"; }

    std::vector<etwin::catalog::DatasetDescriptor> discover() override;
    etwin::catalog::DataChunk fetch(const etwin::catalog::Query& query) override;
    etwin::core::quality::ValidationReport validate(
        const etwin::catalog::DataChunk& chunk) override;
};

} // namespace etwin::connectors
