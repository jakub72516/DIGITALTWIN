#include "connectors/ecological_connector.hpp"

namespace etwin::connectors {

std::string EcologicalConnector::description() const {
    return "Biodiversity and ecological survey ingestion (species occurrences)";
}

std::vector<etwin::catalog::DatasetDescriptor> EcologicalConnector::discover() {
    etwin::catalog::DatasetDescriptor d;
    d.id = "biodiversity_surveys";
    d.name = "Biodiversity Surveys";
    d.description = "Species occurrence records and ecological survey transects";
    d.source_url = "local://ecological";
    d.version = "1.0";
    d.organization = "ETWIN";
    d.license.type = etwin::catalog::LicenseType::Open;
    d.license.name = "Open Data";
    d.spatial.global = false;
    d.spatial.region = etwin::core::GeoRegion{-90.0, 90.0, -180.0, 180.0};
    d.temporal.start = etwin::core::Instant::from_unix_seconds(0);
    d.temporal.end = etwin::core::Instant::from_unix_seconds(4102444800.0);
    d.spatial_resolution_m = 0.0;
    d.variables = {"species_occurrence", "species_richness", "abundance", "cover_fraction"};
    d.update_frequency = "irregular";
    d.quality_status = "partially_validated";
    d.lineage = "field survey -> curation -> occurrence records";
    return {d};
}

etwin::catalog::DataChunk EcologicalConnector::fetch(const etwin::catalog::Query& query) {
    etwin::catalog::DataChunk chunk;
    chunk.dataset_id = "biodiversity_surveys";
    chunk.crs = etwin::core::CRS::epsg_4326();
    chunk.timestamp = query.time_range.start();
    chunk.variable_names = query.variables;
    chunk.shape = {0, query.variables.size()};

    etwin::core::DataSource src;
    src.dataset_id = "biodiversity_surveys";
    src.version = "1.0";
    src.source_url = source_uri();
    src.retrieval_time = etwin::core::Instant::now();
    chunk.provenance = etwin::core::Provenance(src);
    return chunk;
}

etwin::core::quality::ValidationReport EcologicalConnector::validate(
    const etwin::catalog::DataChunk& chunk) {
    etwin::core::quality::ValidationReport report;
    if (chunk.empty()) {
        report.add_issue("EMPTY_CHUNK", "Ecological fetch returned no data",
            etwin::core::quality::Severity::Warning);
    }
    return report;
}

} // namespace etwin::connectors
