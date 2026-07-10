---
state: routed
origin: lab
shipped_pr: null
shipped_repo: null
merged_date: null
outcome: open
---

# Checksum-pin the devkitARM mirror packages (supply-chain hardening)

> **Status:** `ideas`

**Idea (open since session 1):** `tools/setup-toolchain.sh` installs
devkitARM r68 packages from the leseratte10 community mirror, which is
**unsigned** — the pins are name+version only. Add a SHA-256 manifest to the
script (checksums recorded from the artifacts already verified working in
sessions 1–3) and fail the install on mismatch, so a mirror compromise or
silent re-upload can't reach the build path unnoticed.

- **Area:** `tools/setup-toolchain.sh` (single file; the script is already
  the pin manifest — CI cache keys hash it, so adding checksums also
  correctly invalidates the toolchain cache once).
- **Size:** S (sha256sum checks around the existing download steps; record
  current hashes from a known-good install).
- **Risk:** low; worst case is a red CI run pointing at a changed artifact —
  which is exactly the alarm the idea exists to raise.
- **Route:** quick-win — can ride any session's spare capacity; until then
  the caveat stays flagged in `control/status.md`.
