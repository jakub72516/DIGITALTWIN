# End-to-End Demonstration

## Goal

Run a documented, multi-decade scenario involving multiple physical, ecological, and human systems; compare several interventions; publish the run package and validation materials.

## Scenario

- **Period**: 2000-2030 (30 years)
- **Domains**: atmosphere (temperature), hydrology (runoff, soil moisture), agriculture (crop yield), energy (emissions), population (aggregated).
- **Coupling**: atmosphere → hydrology → agriculture; population → energy demand.
- **Interventions**: baseline vs. carbon tax (reduced fossil emission intensity) vs. renewable expansion (increased renewable capacity).

## Run Package Contents

1. `manifest.json` — signed run manifest (models, seeds, parameters, forcing, interventions)
2. `final_state/` — versioned earth state with provenance
3. `validation/` — hindcast metrics (RMSE, bias, Nash-Sutcliffe, correlation)
4. `uncertainty/` — ensemble spread and sensitivity indices
5. `logs/` — audit log and resource monitoring

## Acceptance Criteria

- An external team can repeat the run and verify outputs.
- Assumptions and uncertainty are inspectable.
- Each result is traceable to observations vs. derived vs. simulation.
- Interventions are compared with uncertainty, not a single authoritative answer.

## Reproduction

```
etwin run configs/baseline.json
etwin reproduce run_manifest.json
etwin validate validation/hindcast.json
```
