---
name: quality-gate
description: "Run the project's full verification before pushing and report what must be fixed."
---

# quality-gate

Prove a change is good before pushing gba-homebrew — game-lab Track B: original GBA homebrew on Butano.

1. Run `python3 bootstrap.py check --strict (repo gate); ROM verify = the 'ROM builds' <60s compile check once the walking skeleton lands (conventions rule 16)` — the project's full verification (tests + lint/types).
2. Run `python3 bootstrap.py check --strict` — doc + session-log hygiene. If
   step 1 already IS this command, do not stop at one gate: run the repo's
   remaining gates named in its boot file instead (fleet-manager lesson
   2026-08-04 — the collision silently dropped the false-wall guard).
3. Report every failure with the exact command to reproduce it.
4. Do NOT push on red — green here should mean green in CI.

Declared capabilities: run.
