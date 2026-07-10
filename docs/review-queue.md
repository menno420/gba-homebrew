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
