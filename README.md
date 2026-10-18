# Earth Systems Digital Twin

A continuously updated, multiscale C++20 platform that integrates observations and models of Earth's physical, ecological, and human systems. It supports historical reconstruction and explicitly defined "what if" scenarios, quantifies uncertainty, exposes data and model limitations, and makes each run independently reproducible.

## Status

This is a **validated tool for specified questions and conditions** — not a perfect replica of Earth.

## Architecture

The system is organized around separated components:

| Layer | Purpose | Source |
|-------|---------|--------|
| Data connectors | Retrieve and validate source data | `src/connectors/` |
| Catalog & provenance | Describe data lineage | `src/catalog/`, `src/core/provenance.*` |
| Earth state | Store observations, derived fields, simulation state | `src/state/` |
| Model modules | Simulate specific processes | `src/models/` |
| Coupling | Exchange data between models with checks | `src/coupling/` |
| Scenario & execution | Configure, schedule, checkpoint, run | `src/scenario/`, `src/execution/` |
| Validation & uncertainty | Evaluate results, report sensitivity | `src/validation/` |
| Interfaces & APIs | Expose results, assumptions, provenance | `src/api/` |
| Security | Access control, audit, integrity | `src/security/` |

## Core Types

- **Units** — compile-time dimension checking + runtime registry (`src/core/units.hpp`)
- **Time** — nanosecond instants, ranges, calendars (`src/core/time.hpp`)
- **CRS** — PROJ-backed coordinate reference systems (`src/core/crs.hpp`)
- **Provenance** — content-addressed lineage (`src/core/provenance.hpp`)
- **Spatial** — haversine, bounding boxes, spatial index (`src/core/spatial.hpp`)
- **Quality** — validation reports, quality flags (`src/core/quality.hpp`)
- **Uncertainty** — distributions, ensembles, budgets (`src/core/uncertainty.hpp`)

## Models

| Domain | Model | Source |
|--------|-------|--------|
| Atmosphere | Simple energy balance | `src/models/atmosphere/` |
| Ocean | Layered column | `src/models/ocean/` |
| Hydrology | HBV rainfall-runoff | `src/models/hydrology/` |
| Cryosphere | Degree-day melt | `src/models/cryosphere/` |
| Soil | Multi-layer column | `src/models/soil/` |
| Geohazard | Coulomb stress | `src/models/geohazard/` |
| Ecosystem | Carbon cycle | `src/models/ecosystem/` |
| Agriculture | Crop growth | `src/models/agriculture/` |
| Forestry | Forest gap | `src/models/forestry/` |
| Wildfire | Fire weather/spread | `src/models/wildfire/` |
| Population | Cohort-component (aggregated) | `src/models/population/` |
| Infrastructure | Building stock (aggregated) | `src/models/infrastructure/` |
| Energy | Dispatch | `src/models/energy/` |

## Build

```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build
```

Requires: CMake ≥3.25, a C++20 compiler (Clang/GCC), PROJ, OpenSSL.

## Test Layers

- **Component** — units, time, provenance, spatial, uncertainty
- **Integration** — coupling, scenario engine
- **Validation** — metrics (RMSE, bias, correlation, Nash-Sutcliffe, CRPS)
- **Reproducibility** — deterministic hashing and manifest signatures

## Governance

See `docs/governance/` for the charter, definitions, and operating principles.

## License

See `docs/governance/charter.md` for licensing policy.
