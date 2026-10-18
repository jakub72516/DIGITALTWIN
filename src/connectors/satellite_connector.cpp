#include "connectors/satellite_connector.hpp"

namespace etwin::connectors {

std::string SatelliteConnector::description() const {
    return "Satellite imagery ingestion (optical, radar, multispectral)";
}

std::vector<etwin::catalog::DatasetDescriptor> SatelliteConnector::discover() {
    etwin::catalog::DatasetDescriptor d;
    d.id = "satellite_imagery";
    d.name = "Satellite Imagery";
    d.description = "Multispectral satellite observations (Landsat, Sentinel, MODIS)";
    d.source_url = "local://satellite";
    d.version = "1.0";
    d.organization = "ETWIN";
    d.license.type = etwin::catalog::LicenseType::Open;
    d.license.name = "Open Data";
    d.spatial.global = true;
    d.spatial.region = etwin::core::GeoRegion{-90.0, 90.0, -180.0, 180.0};
    d.temporal.start = etwin::core::Instant::from_unix_seconds(0);
    d.temporal.end = etwin::core::Instant::from_unix_seconds(4102444800.0);
    d.spatial_resolution_m = 30.0;
    d.variables = {"surface_reflectance", "ndvi", "albedo", "land_surface_temperature"};
    d.update_frequency = "daily";
    d.quality_status = "validated";
    d.lineage = "satellite sensor -> atmospheric correction -> surface reflectance";
    return {d};
}

etwin::catalog::DataChunk SatelliteConnector::fetch(const etwin::catalog::Query& query) {
    etwin::catalog::DataChunk chunk;
    chunk.dataset_id = "satellite_imagery";
    chunk.crs = etwin::core::CRS::epsg_4326();
    chunk.timestamp = query.time_range.start();
    chunk.variable_names = query.variables;
    chunk.shape = {0, query.variables.size()};

    etwin::core::DataSource src;
    src.dataset_id = "satellite_imagery";
    src.version = "1.0";
    src.source_url = source_uri();
    src.retrieval_time = etwin::core::Instant::now();
    chunk.provenance = etwin::core::Provenance(src);
    return chunk;
}

etwin::core::quality::ValidationReport SatelliteConnector::validate(
    const etwin::catalog::DataChunk& chunk) {
    etwin::core::quality::ValidationReport report;
    if (chunk.empty()) {
        report.add_issue("EMPTY_CHUNK", "Satellite fetch returned no data",
            etwin::core::quality::Severity::Warning);
    }
    return report;
}

} // namespace etwin::connectors
