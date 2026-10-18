#pragma once

#include "catalog/connector.hpp"

namespace etwin::connectors {

class SeismicConnector : public etwin::catalog::DataConnector {
public:
    std::string name() const override { return "seismic"; }
    std::string description() const override;
    std::string source_uri() const override { return "local://seismic/miniseed"; }

    std::vector<etwin::catalog::DatasetDescriptor> discover() override;
    etwin::catalog::DataChunk fetch(const etwin::catalog::Query& query) override;
    etwin::core::quality::ValidationReport validate(
        const etwin::catalog::DataChunk& chunk) override;
};

} // namespace etwin::connectors
