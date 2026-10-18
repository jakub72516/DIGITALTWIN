#include "connectors/seismic_connector.hpp"

namespace etwin::connectors {

std::string SeismicConnector::description() const {
    return "Seismic waveform ingestion (MiniSEED) and event catalogs";
}

std::vector<etwin::catalog::DatasetDescriptor> SeismicConnector::discover() {
    etwin::catalog::DatasetDescriptor d;
    d.id = "seismic_waveforms";
    d.name = "Seismic Waveforms";
    d.description = "Seismometer waveforms and earthquake event catalogs";
    d.source_url = "local://seismic";
    d.version = "1.0";
    d.organization = "ETWIN";
    d.license.type = etwin::catalog::LicenseType::Open;
    d.license.name = "Open Data";
    d.spatial.global = false;
    d.spatial.region = etwin::core::GeoRegion{-90.0, 90.0, -180.0, 180.0};
    d.temporal.start = etwin::core::Instant::from_unix_seconds(0);
    d.temporal.end = etwin::core::Instant::from_unix_seconds(4102444800.0);
    d.spatial_resolution_m = 0.0;
    d.variables = {"ground_motion", "event_magnitude", "event_location", "event_depth"};
    d.update_frequency = "continuous";
    d.quality_status = "validated";
    d.lineage = "seismometer -> MiniSEED -> event association";
    return {d};
}

etwin::catalog::DataChunk SeismicConnector::fetch(const etwin::catalog::Query& query) {
    etwin::catalog::DataChunk chunk;
    chunk.dataset_id = "seismic_waveforms";
    chunk.crs = etwin::core::CRS::epsg_4326();
    chunk.timestamp = query.time_range.start();
    chunk.variable_names = query.variables;
    chunk.shape = {0, query.variables.size()};

    etwin::core::DataSource src;
    src.dataset_id = "seismic_waveforms";
    src.version = "1.0";
    src.source_url = source_uri();
    src.retrieval_time = etwin::core::Instant::now();
    chunk.provenance = etwin::core::Provenance(src);
    return chunk;
}

etwin::core::quality::ValidationReport SeismicConnector::validate(
    const etwin::catalog::DataChunk& chunk) {
    etwin::core::quality::ValidationReport report;
    if (chunk.empty()) {
        report.add_issue("EMPTY_CHUNK", "Seismic fetch returned no data",
            etwin::core::quality::Severity::Warning);
    }
    return report;
}

} // namespace etwin::connectors
