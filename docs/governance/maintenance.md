# Governance, Maintenance, and Continual Expansion

## Model and Dataset Review

- All new models and datasets require review and approval before registration.
- Review includes: scientific validity, data governance, license compliance, and security.
- Approved changes are versioned and recorded in the catalog.

## Versioning

- Datasets: content-addressed with documented lineage.
- Models: semantic versioning (`major.minor`).
- Configuration: versioned in source control.
- Run outputs: tied to run manifest hashes.

## Deprecation Policy

- Deprecated models remain runnable for reproducibility.
- Deprecated datasets remain queryable but flagged as superseded.
- Older runs remain interpretable via preserved manifests and code versions.

## Incident Response

- Data quality failures: flag and report; never silently drop data.
- Security events: audit log review, access revocation, integrity verification.
- Run failures: mark as failed; no partial output presented as success.

## Update Policy

- Updates are traceable through the audit log.
- New capabilities are not presented as validated until their review gates pass.

## External Scientific Review

- Independent reviewers can reproduce validation runs.
- Claims are restricted to what evidence supports.

## Expansion Process

1. Propose new model or dataset.
2. Submit for review (scientific + data governance + security).
3. Implement and validate.
4. Register in catalog/factory.
5. Document and deprecate appropriately.
