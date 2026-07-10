# gba-homebrew (game-lab Track B) · status

updated: 2026-07-10T06:41:00Z
phase: session 7 WIP — Lumen Drift polish pass 3: "more cave beyond row 62", the concept doc's LAST unbuilt polish-list item (claim `claims/lumen-drift-polish-3.md`)
health: green
kit: v1.6.0 · check: green (--strict exit 0 at session start on `d9acaa5`) · engaged: yes
last-shipped: PR #20 — Lumen Drift polish pass 2 (merge `534a7ca`); PR #21 — session-6 close-out (merge `d9acaa5`)
blockers: none
orders: acked=001 done=001 (this repo's half; joint done-when closes when Track A's half lands in pokemon-mod-lab) · no new orders at HEAD (inbox re-read 2026-07-10T06:40Z, HEAD `d9acaa5`) · session 7 runs the standing default under coordinator direction: the one remaining concept-doc polish item (more cave beyond row 62)
⚑ needs-owner: concept pick — WHAT: pick 1 of the 3 committed Track B concepts (Lumen Drift / Clockwork Courier / Shoal), jointly with Track A's 3 per ORDER 001. WHERE: docs/concepts/session-1-concepts.md. HOW: any signal (inbox order or PR comment). WHY-IT-MATTERS: gates which game gets the remaining sessions — Lumen Drift is scope-complete with two polish passes and pass 3 (the list's final item) in flight this session. VERIFIED-NEEDED: yes — ORDER 001 reserves the pick to the owner. NOTE: everything generic transfers (games/common/ headers, harness scripting/text/watch assertions/savefile persistence, all-games CI). UNBLOCKS: post-pick session planning — after this session the Lumen Drift polish list is exhausted, so the pick decides transfer-vs-new-direction with zero sunk work.
notes: session 7 in flight on branch game/lumen-drift-polish-3 (heartbeat first). Walls carried: api.github.com curl proxy-walled (GitHub MCP is the merge path); mGBA core.load_save() segfault (PLATFORM-LIMITS.md — --savefile bus-copy is the working path). Supply chain CLOSED to trust-on-first-use (PR #13 pins). Guard recipe in force: any restart()-stall change shifts every replay span by a constant — loop-tick watch, then re-bisect (sessions 5/6).
