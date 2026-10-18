#include "connectors/population_connector.hpp"

namespace etwin::connectors {

std::string PopulationConnector::description() const {
    return "Aggregated population raster ingestion (privacy-preserving)";
}

std::vector<etwin::catalog::DatasetDescriptor> PopulationConnector::discover() {
    etwin::catalog::DatasetDescriptor d;
    d.id = "population_rasters";
    d.name = "Population Rasters";
    d.description = "Aggregated population distribution (administrative units only)";
    d.source_url = "local://population";
    d.version = "1.0";
    d.organization = "ETWIN";
    d.license.type = etwin::catalog::LicenseType::Restricted;
    d.license.name = "Restricted - aggregated only";
    d.license.allows_redistribution = true;
    d.spatial.global = false;
    d.spatial.region = etwin::core::GeoRegion{-90.0, 90.0, -180.0, 180.0};
    d.temporal.start = etwin::core::Instant::from_unix_seconds(0);
    d.temporal.end = etwin::core::Instant::from_unix_seconds(4102444800.0);
    d.spatial_resolution_m = 1000.0;
    d.variables = {"population_density", "population_count"};
    d.update_frequency = "annual";
    d.quality_status = "validated";
    d.lineage = "census -> spatial downscaling -> aggregated raster";
    return {d};
}

etwin::catalog::DataChunk PopulationConnector::fetch(const etwin::catalog::Query& query) {
    etwin::catalog::DataChunk chunk;
    chunk.dataset_id = "population_rasters";
    chunk.crs = etwin::core::CRS::epsg_4326();
    chunk.timestamp = query.time_range.start();
    chunk.variable_names = query.variables;
    chunk.shape = {0, query.variables.size()};

    etwin::core::DataSource src;
    src.dataset_id = "population_rasters";
    src.version = "1.0";
    src.source_url = source_uri();
    src.retrieval_time = etwin::core::Instant::now();
    chunk.provenance = etwin::core::Provenance(src);
    return chunk;
}

etwin::core::quality::ValidationReport PopulationConnector::validate(
    const etwin::catalog::DataChunk& chunk) {
    etwin::core::quality::ValidationReport report;
    if (chunk.empty()) {
        report.add_issue("EMPTY_CHUNK", "Population fetch returned no data",
            etwin::core::quality::Severity::Warning);
    }
    return report;
}

} // namespace etwin::connectors
