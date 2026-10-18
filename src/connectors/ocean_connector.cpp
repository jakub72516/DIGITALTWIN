#include "connectors/ocean_connector.hpp"

namespace etwin::connectors {

std::string OceanConnector::description() const {
    return "Ocean observation ingestion (Argo profiles, buoys, SST)";
}

std::vector<etwin::catalog::DatasetDescriptor> OceanConnector::discover() {
    etwin::catalog::DatasetDescriptor d;
    d.id = "ocean_observations";
    d.name = "Ocean Observations";
    d.description = "Argo float profiles, buoy measurements, sea surface temperature";
    d.source_url = "local://ocean";
    d.version = "1.0";
    d.organization = "ETWIN";
    d.license.type = etwin::catalog::LicenseType::Open;
    d.license.name = "Open Data";
    d.spatial.global = false;
    d.spatial.region = etwin::core::GeoRegion{-90.0, 90.0, -180.0, 180.0};
    d.temporal.start = etwin::core::Instant::from_unix_seconds(0);
    d.temporal.end = etwin::core::Instant::from_unix_seconds(4102444800.0);
    d.spatial_resolution_m = 0.0;
    d.variables = {"sst", "salinity", "temperature_profile", "current_velocity"};
    d.update_frequency = "10-day cycle";
    d.quality_status = "validated";
    d.lineage = "Argo float -> QC -> gridded product";
    return {d};
}

etwin::catalog::DataChunk OceanConnector::fetch(const etwin::catalog::Query& query) {
    etwin::catalog::DataChunk chunk;
    chunk.dataset_id = "ocean_observations";
    chunk.crs = etwin::core::CRS::epsg_4326();
    chunk.timestamp = query.time_range.start();
    chunk.variable_names = query.variables;
    chunk.shape = {0, query.variables.size()};

    etwin::core::DataSource src;
    src.dataset_id = "ocean_observations";
    src.version = "1.0";
    src.source_url = source_uri();
    src.retrieval_time = etwin::core::Instant::now();
    chunk.provenance = etwin::core::Provenance(src);
    return chunk;
}

etwin::core::quality::ValidationReport OceanConnector::validate(
    const etwin::catalog::DataChunk& chunk) {
    etwin::core::quality::ValidationReport report;
    if (chunk.empty()) {
        report.add_issue("EMPTY_CHUNK", "Ocean fetch returned no data",
            etwin::core::quality::Severity::Warning);
    }
    return report;
}

} // namespace etwin::connectors
