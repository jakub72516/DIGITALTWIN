# Charter

## Mission

Build a continuously updated, multiscale C++ platform that integrates observations and models of Earth's physical, ecological, and human systems. The system supports historical reconstruction and explicitly defined "what if" scenarios, quantifies uncertainty, exposes data and model limitations, and makes each run independently reproducible.

## Purpose

The system is a traceable, testable way to explore scenarios and understand how assumptions affect results. It is a **validated tool for specified questions and conditions**, not a perfect replica of Earth.

## Intended Users

- **Scientists**: model development, hindcast validation, scenario analysis
- **Operational users**: monitoring, early warning, resource planning
- **Analysts and policymakers**: conditional future exploration, intervention comparison
- **Auditors and reviewers**: reproducibility checks, provenance inspection

## Prohibited Claims

1. The system MUST NOT be presented as a "perfect replica of Earth."
2. The system MUST NOT present scenario outputs as predictions or certainties.
3. The system MUST NOT claim validation beyond what evidence supports.
4. The system MUST NOT present partial or failed output as successful results.
5. The system MUST NOT identify individuals or expose sensitive infrastructure details.
6. The system MUST NOT present alternative models as interchangeable without flagging their differences.

## Definitions

- **Global**: coverage where data permit; finer detail where available. Not necessarily complete Earth coverage.
- **Real time**: ingestion latency relative to source update frequency, explicitly reported per dataset.
- **Reproducible**: same inputs, model versions, configuration, and seeds produce bitwise-identical outputs.
- **Validated**: a model has passed its defined validation gates for stated conditions; not generally true.

## Data Governance

- All ingested data carries provenance: source, license, coverage, resolution, quality, lineage.
- License compatibility is checked at ingestion time, not documented afterward.
- Sensitive data classification is enforced at ingestion and query time.
- Missing, conflicting, delayed, and malformed data are reported, never silently hidden.

## Privacy Boundaries

- Population and human-system data are aggregated to administrative units only.
- Infrastructure data are aggregated to zones; individual locations are not exposed.
- Outputs pass a sensitivity screen before release.

## Scientific Review

- New models and validation claims require external scientific review.
- Each model documents: equations, valid conditions, limitations, calibration, failure modes.
- Results signal when available evidence is insufficient.

## Service Goals (measurable)

- Data ingestion: no silent data loss; all gaps reported.
- Reproducibility: bitwise regression tests in CI.
- Validation: all claims tied to defined metrics and thresholds.
- Security: no critical integrity failure can silently appear as a successful run.

## Operating Principles

- Every result is tied to inputs, model versions, configuration, seeds, and limitations.
- Model modules are replaceable; alternative models coexist for comparison.
- Uncertainty and known limitations are recorded with results.
