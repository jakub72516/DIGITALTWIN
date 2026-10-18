# Architecture Decision Records

## ADR-001: Modern C++20 with CMake

**Status**: Accepted

**Context**: The platform requires strong type safety, performance, and cross-platform support for scientific computing.

**Decision**: Use C++20 with CMake as the build system. Compile with Clang (primary) and GCC (CI).

**Consequences**: Requires modern compiler toolchain; enables concepts, ranges, and `std::format`.

---

## ADR-002: Compile-time and Runtime Unit System

**Status**: Accepted

**Context**: Unit errors are a common source of scientific bugs. Some unit resolution must happen at runtime (external data).

**Decision**: Hybrid approach — compile-time dimension checking for internal types, runtime unit registry for dynamic conversions.

**Consequences**: Catches most unit errors at compile time; runtime registry handles dynamic external data.

---

## ADR-003: Content-Addressed State with Provenance

**Status**: Accepted

**Context**: Reproducibility requires every result to be traceable to inputs.

**Decision**: Earth state is versioned and content-addressed. Every field carries provenance (source, transform chain, quality).

**Consequences**: Storage overhead from hashes and provenance; but guarantees auditability and reproducibility.

---

## ADR-004: Replaceable Model Modules via Factory Pattern

**Status**: Accepted

**Context**: The platform must compare alternative models rather than treating one as truth.

**Decision**: All models implement a common `ModelModule` interface and are created via a factory.

**Consequences**: Models are swappable; coupling checks happen at the interface boundary.

---

## ADR-005: Declarative Coupling Graph

**Status**: Accepted

**Context**: Coupling must be explicit, debuggable, and auditable.

**Decision**: Coupling links are declared as source→target field mappings with explicit units, timesteps, and resolutions.

**Consequences**: Coupling errors are caught at configuration time; runtime checks flag unsupported conditions.

---

## ADR-006: Scenario-as-Code with Signed Manifests

**Status**: Accepted

**Context**: Scenarios must be versionable, reviewable, and tamper-evident.

**Decision**: Scenarios are JSON/YAML configurations; run manifests are signed and immutable.

**Consequences**: Enables independent reproduction and audit.

---

## ADR-007: Explicit Uncertainty Propagation

**Status**: Accepted

**Context**: Results must not present a single authoritative-looking answer.

**Decision**: Uncertainty is propagated via distributions, ensembles, and budgets; reported alongside results.

**Consequences**: Additional compute for ensembles; but prevents false certainty.

---

## ADR-008: Fail-Visible Run Semantics

**Status**: Accepted

**Context**: Partial output must never appear successful.

**Decision**: Runs are marked `failed` on any error; no partial output is presented as complete.

**Consequences**: Strict run lifecycle; checkpoint-restart preserves reproducibility.
