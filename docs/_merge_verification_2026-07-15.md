# Merge-automation verification probe

This file was added on 2026-07-15 as a merge-automation verification probe: a
tiny, inert, plain-content change used to confirm that ordinary (non-workflow-file)
code/doc PRs in this repository land on green CI with zero human click, via the
existing native GitHub auto-merge mechanism armed by `.github/workflows/auto-merge-enabler.yml`.

No functional change is intended. This file may be deleted in a future cleanup
once the verification is recorded.
