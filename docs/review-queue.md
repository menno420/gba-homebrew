# Review queue — post-merge review ledger

> **Status:** `living-ledger` — the merge-then-flag half of the self-merge
> grant ([`conventions.md`](conventions.md) rule 3; fleet merge-authority
> policy, 2026-07-09). Nothing waits for review: PRs merge on green, and
> anything a reviewer should re-check gets a row here (and/or a Codex
> @-mention on the PR). Review is post-merge; veto = revert. An empty queue
> with all PRs merged is the healthy state.

| PR | What to re-check | Why |
|---|---|---|
| #3 | `tools/setup-toolchain.sh` pins + supply-chain caveat; `games/skeleton` originality; "ROM builds" workflow correctness | Walking skeleton PR: unsigned community mirror (leseratte10) enters the build path; the CI check becomes the repo merge gate |
