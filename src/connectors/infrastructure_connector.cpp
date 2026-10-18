#include "connectors/infrastructure_connector.hpp"

namespace etwin::connectors {

std::string InfrastructureConnector::description() const {
    return "Building, transport, and energy infrastructure ingestion (aggregated)";
}

std::vector<etwin::catalog::DatasetDescriptor> InfrastructureConnector::discover() {
    etwin::catalog::DatasetDescriptor d;
    d.id = "infrastructure";
    d.name = "Infrastructure";
    d.description = "Buildings, roads, transport, and energy infrastructure (aggregated)";
    d.source_url = "local://infrastructure";
    d.version = "1.0";
    d.organization = "ETWIN";
    d.license.type = etwin::catalog::LicenseType::Open;
    d.license.name = "Open Data";
    d.spatial.global = false;
    d.spatial.region = etwin::core::GeoRegion{-90.0, 90.0, -180.0, 180.0};
    d.temporal.start = etwin::core::Instant::from_unix_seconds(0);
    d.temporal.end = etwin::core::Instant::from_unix_seconds(4102444800.0);
    d.spatial_resolution_m = 0.0;
    d.variables = {"building_count", "road_length", "grid_capacity", "water_pipe_length"};
    d.update_frequency = "irregular";
    d.quality_status = "partially_validated";
    d.lineage = "OSM -> spatial aggregation -> zonal summaries";
    return {d};
}

etwin::catalog::DataChunk InfrastructureConnector::fetch(const etwin::catalog::Query& query) {
    etwin::catalog::DataChunk chunk;
    chunk.dataset_id = "infrastructure";
    chunk.crs = etwin::core::CRS::epsg_4326();
    chunk.timestamp = query.time_range.start();
    chunk.variable_names = query.variables;
    chunk.shape = {0, query.variables.size()};

    etwin::core::DataSource src;
    src.dataset_id = "infrastructure";
    src.version = "1.0";
    src.source_url = source_uri();
    src.retrieval_time = etwin::core::Instant::now();
    chunk.provenance = etwin::core::Provenance(src);
    return chunk;
}

etwin::core::quality::ValidationReport InfrastructureConnector::validate(
    const etwin::catalog::DataChunk& chunk) {
    etwin::core::quality::ValidationReport report;
    if (chunk.empty()) {
        report.add_issue("EMPTY_CHUNK", "Infrastructure fetch returned no data",
            etwin::core::quality::Severity::Warning);
    }
    return report;
}

} // namespace etwin::connectors
