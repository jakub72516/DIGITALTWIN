#include "connectors/weather_connector.hpp"

namespace etwin::connectors {

std::string WeatherConnector::description() const {
    return "Weather station observation ingestion (CSV/NetCDF)";
}

std::vector<etwin::catalog::DatasetDescriptor> WeatherConnector::discover() {
    etwin::catalog::DatasetDescriptor d;
    d.id = "weather_stations";
    d.name = "Surface Weather Stations";
    d.description = "Surface temperature, precipitation, humidity, wind observations";
    d.source_url = "local://weather";
    d.version = "1.0";
    d.organization = "ETWIN";
    d.license.type = etwin::catalog::LicenseType::Open;
    d.license.name = "Open Data";
    d.spatial.global = true;
    d.spatial.region = etwin::core::GeoRegion{-90.0, 90.0, -180.0, 180.0};
    d.temporal.start = etwin::core::Instant::from_unix_seconds(0);
    d.temporal.end = etwin::core::Instant::from_unix_seconds(4102444800.0);
    d.spatial_resolution_m = 0.0;
    d.variables = {"temperature", "precipitation", "humidity", "wind_speed", "pressure"};
    d.update_frequency = "hourly";
    d.quality_status = "validated";
    d.lineage = "direct station observation";
    return {d};
}

etwin::catalog::DataChunk WeatherConnector::fetch(const etwin::catalog::Query& query) {
    etwin::catalog::DataChunk chunk;
    chunk.dataset_id = "weather_stations";
    chunk.crs = etwin::core::CRS::epsg_4326();
    chunk.timestamp = query.time_range.start();
    chunk.variable_names = query.variables;
    chunk.shape = {0, query.variables.size()};

    etwin::core::DataSource src;
    src.dataset_id = "weather_stations";
    src.version = "1.0";
    src.source_url = source_uri();
    src.retrieval_time = etwin::core::Instant::now();
    chunk.provenance = etwin::core::Provenance(src);
    return chunk;
}

etwin::core::quality::ValidationReport WeatherConnector::validate(
    const etwin::catalog::DataChunk& chunk) {
    etwin::core::quality::ValidationReport report;
    if (chunk.empty()) {
        report.add_issue("EMPTY_CHUNK", "Weather fetch returned no data",
            etwin::core::quality::Severity::Warning);
    }
    return report;
}

} // namespace etwin::connectors
