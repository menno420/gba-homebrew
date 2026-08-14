# 2026-08-13 — substrate-kit v1.20.1 → v1.21.0 (distribution wave, phase 3)

> **Status:** `complete` — branch `claude/substrate-kit-v1-21-0`, PR #215.
> This flip closes the born-red hold. The upgrade was complete and reviewed at
> `445948b2` on 2026-08-13; the PR then parked on the BlocksDS toolchain
> rotation (required `NDS ROM build` broken on every PR, pre-existing). It
> merges 2026-08-14 under an owner-authorized one-time bypass of that one
> context — its red is the upstream 404, measured unrelated to this docs/kit
> diff — with `ROM builds` still honored green and the requirement restored
> immediately after (record on the PR). The 1.22.3 toolchain migration is
> PR #216's own thread.

- **📊 Model:** fable-5 · high · mechanical refactor

## previous-session review

Same session, earlier turn: the phase-3 wave landed four adopters
tree-verified and parked this one on the toolchain fork — that record
(fleet-manager `.sessions/2026-08-13-kit-v1210-rollout-phase3.md`) matches
what this branch found; nothing contradicted.

## Shipped

- Vendored dist v1.20.1 → v1.21.0 (sha256 `8807a00e…9cc7356` four ways),
  pin → 1.21.0, `kit:` line → v1.21.0; rollback banked byte-identical.
- Enabler regen REVERTED (host card guard preserved); gate regen kept
  (purely additive). Two arc-doc badges tokenized; one reasoned allowlist
  entry. All measured pre-existing under the banked v1.20.1 dist.

## Verify

- `check --strict`: red on exactly the designed hold pre-flip; the three
  gate findings resolved; Codex round 1 `[conceded]` 5/5, all dist-routed.
- Merge context: owner-authorized bypass of `NDS ROM build` only (broken by
  the upstream toolchain rotation, evidence on PR #216); `ROM builds` green
  honored on the flip head.

💡 A required check that can be broken by an external server deleting a file
is a standing outage risk — PR #216's mirror release pattern (hash-pinned,
self-hosted) is the durable answer and should extend to the other rolling
sources when the migration session runs.
