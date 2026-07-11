# Review queue — post-merge review ledger

> **Status:** `living-ledger` — the merge-then-flag half of the self-merge
> grant ([`conventions.md`](conventions.md) rule 3; fleet merge-authority
> policy, 2026-07-09). Nothing waits for review: PRs merge on green, and
> anything a reviewer should re-check gets a row here (and/or a Codex
> @-mention on the PR). Review is post-merge; veto = revert. An empty queue
> with all PRs merged is the healthy state.
>
> Closed rows are DELETED (git history is the archive); each close-out's
> verdicts live on the closing session's card. Rows #3, #5, #6, #8, #9, #12,
> #13, #16, #17, #20 and #29 were verified and closed 2026-07-11 by session 8
> slice 2 (verdicts + evidence: `.sessions/2026-07-11-session-8.md`; two real
> defects found and fixed in the same PR). Row #23 below is the NARROWED
> remainder of the original row #23 — everything else in it verified.

| PR | What to re-check | Why |
|---|---|---|
| #23 | Echo-band crystal gates (found 2026-07-11, session 8 slice 2): 6 junctions in rows 191–1093 (first at row 191) where EVERY shared open column between consecutive rows carries a crystal in one of the two rows — an unavoidable-death gate for the 6px mote descending the endless echoes. Host-side mirror of `layout_of_row`/`row_solid`/`row_features` proved it; committed rows 0–63 all keep ≥1 crystal-free shared column, and both CI replay tiers die far above row 191, so no pinned proof catches this. A fix must change `row_features` to spare one junction column (pure, neighbor-aware) and revalidate the deep replay — its own slice, gated on the owner concept pick since it rewrites endless-content hazards | The original row #23's solidity-only passability argument ("adjacent centers ≤3 cols, bands ≥2 half-cells") verified TRUE for rows 5–1200 — but it never covered the crystal overlay. Everything else in the old row verified 2026-07-11: pure-row purity + tripwires (both replay tiers re-run locally, green), `_taken` 24-cap eviction, telemetry hook slots 4-6, the +4 offset (canonical replay green on today's build) |
