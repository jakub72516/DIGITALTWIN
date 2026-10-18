# Data Connector Documentation

## Connector Specification

Each data connector implements the `DataConnector` interface:
- `discover()` — list available datasets with metadata
- `fetch(query)` — retrieve data for a spatial/temporal/variable query
- `validate(chunk)` — report data quality issues

## Connectors

| Connector | Domain | Format | License |
|-----------|--------|--------|---------|
| weather | Weather stations | CSV/NetCDF | Open |
| satellite | Satellite imagery | Raster (GDAL) | Open |
| ocean | Ocean profiles | Argo/buoy | Open |
| seismic | Seismic waveforms | MiniSEED | Open |
| ecological | Biodiversity | GBIF | Open |
| agricultural | Crop statistics | Tabular | Open |
| population | Population | Aggregated raster | Restricted |
| infrastructure | Buildings/transport/energy | OSM (aggregated) | Open |

## Provenance Requirements

Every dataset must document:
- Source URL and version
- License and redistribution terms
- Spatial and temporal coverage
- Resolution and update frequency
- Quality status and lineage

## Ingestion Policy

- License compatibility is checked before ingestion
- Missing, conflicting, delayed, and malformed data are reported
- No silent data loss or gap-filling without flagging
