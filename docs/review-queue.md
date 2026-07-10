# Review queue — post-merge review ledger

> **Status:** `living-ledger` — the merge-then-flag half of the self-merge
> grant ([`conventions.md`](conventions.md) rule 3; fleet merge-authority
> policy, 2026-07-09). Nothing waits for review: PRs merge on green, and
> anything a reviewer should re-check gets a row here (and/or a Codex
> @-mention on the PR). Review is post-merge; veto = revert. An empty queue
> with all PRs merged is the healthy state.

| PR | What to re-check | Why |
|---|---|---|
| #3 | `tools/setup-toolchain.sh` pins + supply-chain caveat; `games/skeleton` originality; "ROM builds" workflow correctness | Walking skeleton PR: unsigned community mirror (leseratte10) enters the build path; the CI checks become the repo merge gate (incl. the substrate-gate no-card sentinel fix) |
| #5 | substrate-gate born-red-card rule: an ADDED card gates advisory, a MODIFIED card keeps the locked door — confirm a close-out that forgets the `complete` flip still reds | Second live-fire gate fix (heartbeat PRs could never merge green; PR #2 merged red because of it); worth upstreaming to substrate-kit with the PR #3 fix |
| #6 | `games/lumen-drift` + `games/common` originality and the generic/specific split; `gl_tile_grid.h` collision edge cases (cell-boundary epsilon, negative coords); `--require-distinct` threshold (100 px) | First gameplay PR; the shared scaffolding is the bet that survives the owner's pending concept pick — review it as engine code, not game code |
| #8 | Heartbeat rode the PR #5 born-red-card gate rule with zero friction — spot-check the gate log agrees the card was ADDED (advisory), not MODIFIED | First live run of the fixed rule without a same-PR gate change muddying the evidence |
| #9 | Hazard-kill fairness: `hazard_half` (2) vs `mote_half` (3) forgiveness margin; surge kill line vs its visible front row (an off-by-a-row would kill invisibly); `gl_run_state.h` reviewed as engine code; hazard tiles originality | Fail-state PR: kill conditions are the new player-facing contract; the run-state header is the transferable bet |
| #12 | Section-boundary tunnel continuity (the deep's +8 wave phase offset exists so row 44 lines up under row 43 — verify no unreachable/softlock pocket); shard `pickup_half` (4) generosity vs `hazard_half` (2); `gl_stage.h` + `visit_overlaps` reviewed as engine code; shard/rim tiles originality | Scope-completing gameplay PR: cave generation changed for rows 24+; pickups are a new player-facing contract |
| #13 | SHA-256 pins recorded by this lane from its own known-good artifacts (trust-on-first-use, not publisher-signed) — independently re-derive a hash or two if warranted; `--assert-text` ink-color assumptions (white/black glyphs, bg-fade-immune) if fonts/palettes ever change; `lumen-drift-proof` job stays dispatch-tier only | Supply-chain posture change + new CI assertion mechanism; the pins are only as good as the first-use snapshot |
