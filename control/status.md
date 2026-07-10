# gba-homebrew (game-lab Track B) · status

updated: 2026-07-10T03:39:41Z
phase: session 4 WIP — Lumen Drift increment 3 (shards + cave sections, the concept doc's remaining committed scope) + routed quick-wins (HUD-depth CI assert, toolchain checksum pins)
health: green
kit: v1.6.0 · check: green at session start (full --strict exit 0) · engaged: yes
last-shipped: PR #9 — Lumen Drift increment 2 (hazards + fail state); close-out PR #10
blockers: none
orders: acked=001 done=001 (this repo's half; joint done-when closes when Track A's half lands in pokemon-mod-lab) · no new orders at HEAD (inbox re-read 2026-07-10T03:39:41Z, HEAD 28b7a30) · session 4 runs the standing default: Lumen Drift increment 3 (coordinator-directed; claim claims/game-lumen-drift-increment-3.md)
⚑ needs-owner: concept pick — WHAT: pick 1 of the 3 committed Track B concepts (Lumen Drift / Clockwork Courier / Shoal), jointly with Track A's 3 per ORDER 001. WHERE: docs/concepts/session-1-concepts.md (estimates refreshed against measured increments 1–2). HOW: any signal (inbox order or PR comment). WHY-IT-MATTERS: gates which game gets the remaining sessions. VERIFIED-NEEDED: yes — ORDER 001 reserves the pick to the owner. NOTE: increments 1–3 built as Lumen Drift under the standing default (coordinator directive); after increment 3 the concept doc's committed scope is complete, so the pick decides polish-vs-transfer, not sunk work. UNBLOCKS: locks the target for post-first-complete polish (Lumen Drift) or transfers the generic scaffolding (games/common/ input/kinematics/collision/run-state, harness input scripting, all-games CI) to the picked concept and retires the generic-scaffolding hedge.
notes: Session 4 WIP heartbeat. Supply-chain caveat (unsigned leseratte10 mirror) is this session's routed quick-win target (docs/ideas/toolchain-checksum-pin-2026-07-10.md). ⚑ upstream to substrate-kit still carried: both substrate-gate fixes (PR #3 no-card sentinel, PR #5 added-card-advisory).
