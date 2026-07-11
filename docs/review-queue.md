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
> defects found and fixed in the same PR). The narrowed row #23 (echo-band
> crystal gates) was FIXED and closed 2026-07-11 by session 8 slice 3:
> neighbour-aware junction guard in `row_features` + full-period host proof
> `tools/check-cave.py` now running per PR in the "ROM builds" gate
> (verdict + evidence on the same card). **The queue is EMPTY — healthy
> state.**

| PR | What to re-check | Why |
|---|---|---|
