# Model Documentation Standard

Every model module must document the following before it can be registered as validated.

## Required Sections

1. **Physical/mathematical description** — equations or methods used
2. **Valid conditions** — the domain where the model is trustworthy
3. **Limitations** — what the model cannot do
4. **Calibration procedure** — how parameters are determined
5. **Validation results** — hindcast metrics and thresholds
6. **Known failure modes** — conditions under which outputs are untrustworthy
7. **Coupling interface** — inputs required and outputs produced

## Registration Gate

A model may not be presented as validated until:
- Its validation suite passes defined thresholds
- An independent reviewer has approved its documentation
- Its failure modes are enumerated and runtime-checked

## Runtime Enforcement

The execution framework checks whether a model is operating within its declared valid conditions. When a model operates outside its validated envelope, the run records a warning and the results carry a reduced validity score.

## Deprecation

Models are versioned. When a model is superseded, it is deprecated (still runnable for reproducibility) rather than deleted.
