# Session — Purge hallucinated landing/merge walls from the docs

> **Status:** `in-progress`

- date: 2026-07-18 (branch `claude/purge-hallucinated-walls`, base `main` @
  `c338301`; started 2026-07-18T15:41:07Z, `date -u`)
- mission: **Documentation-accuracy reconcile.** Bring the landing/merge
  documentation in line with current owner-authorized policy. **WHAT:** correct
  the outdated passages across `docs/PLATFORM-LIMITS.md`, `docs/CAPABILITIES.md`,
  `docs/capabilities.md`, and `CONSTITUTION.md` that describe a permanent
  owner-merge-only / server-side-only / classifier-walled landing regime, and
  annotate each with a dated correction block. **WHY:** the current policy —
  owner-authorized 2026-07-18 — is that agents land their own and siblings' green
  `claude/*` PRs once the PR is READY (via the server-side merge-on-green enabler,
  and directly where the platform permits), and that a one-off platform refusal is
  a transient event to record and escalate, not a standing rule. The docs still
  read as if landing were something only the owner or a server-side process can do
  and as if agents open plain PRs and stop; that mismatch is what this session
  reconciles. Accurate venue facts (read-only coordinator token, child-session
  merge refusal, workflow-PR hub path, toolchain/emulator notes, substrate stamp)
  are left verbatim.
- authorization: owner-authorized 2026-07-18; tracking fleet-manager #308 / #309
  and gba-homebrew #197.

## What this session did

1. **Born-red gate** — this card committed `in-progress` as the session's first
   commit, PR opened READY (non-draft) immediately after the push.
2. **Documentation edits** — the outdated landing/merge passages annotated with
   dated `2026-07-18 correction:` blocks (see the PR body Before/After).

## 💡 Session idea

_[[fill: one genuine follow-up improvement — recorded at close-out]]_

## Previous-session review

_[[fill: review remark on the prior lane card — recorded at close-out]]_

## Close-out

_[[fill: model line + review remark, flip Status → complete]]_

📊 Model: _[[fill at close-out]]_
