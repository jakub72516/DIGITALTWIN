#include "connectors/agricultural_connector.hpp"

namespace etwin::connectors {

std::string AgriculturalConnector::description() const {
    return "Agricultural statistics ingestion (crop yields, planted area)";
}

std::vector<etwin::catalog::DatasetDescriptor> AgriculturalConnector::discover() {
    etwin::catalog::DatasetDescriptor d;
    d.id = "agricultural_statistics";
    d.name = "Agricultural Statistics";
    d.description = "Crop yields, planted area, and management practice statistics";
    d.source_url = "local://agriculture";
    d.version = "1.0";
    d.organization = "ETWIN";
    d.license.type = etwin::catalog::LicenseType::Open;
    d.license.name = "Open Data";
    d.spatial.global = false;
    d.spatial.region = etwin::core::GeoRegion{-90.0, 90.0, -180.0, 180.0};
    d.temporal.start = etwin::core::Instant::from_unix_seconds(0);
    d.temporal.end = etwin::core::Instant::from_unix_seconds(4102444800.0);
    d.spatial_resolution_m = 0.0;
    d.variables = {"crop_yield", "planted_area", "harvested_area", "fertilizer_use"};
    d.update_frequency = "annual";
    d.quality_status = "validated";
    d.lineage = "national/regional statistics -> aggregation";
    return {d};
}

etwin::catalog::DataChunk AgriculturalConnector::fetch(const etwin::catalog::Query& query) {
    etwin::catalog::DataChunk chunk;
    chunk.dataset_id = "agricultural_statistics";
    chunk.crs = etwin::core::CRS::epsg_4326();
    chunk.timestamp = query.time_range.start();
    chunk.variable_names = query.variables;
    chunk.shape = {0, query.variables.size()};

    etwin::core::DataSource src;
    src.dataset_id = "agricultural_statistics";
    src.version = "1.0";
    src.source_url = source_uri();
    src.retrieval_time = etwin::core::Instant::now();
    chunk.provenance = etwin::core::Provenance(src);
    return chunk;
}

etwin::core::quality::ValidationReport AgriculturalConnector::validate(
    const etwin::catalog::DataChunk& chunk) {
    etwin::core::quality::ValidationReport report;
    if (chunk.empty()) {
        report.add_issue("EMPTY_CHUNK", "Agricultural fetch returned no data",
            etwin::core::quality::Severity::Warning);
    }
    return report;
}

} // namespace etwin::connectors
