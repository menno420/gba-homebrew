# Session 43 — game-lab Track B

> **Status:** `in-progress`

- date: 2026-07-13 (branch `claude/brineward-saves`, started 18:22Z)
- mission: **BRINEWARD arc slice 9 — SAVES.** The next (and final
  named) roadmap beat in `docs/concepts/brineward-concept.md` @ main
  `97e0117` (loot/gold → port+upgrades → the Maw → wind → danger
  bands → audio → **saves**), item 7 verbatim: "saves: banked gold /
  upgrades / chart persist (SRAM pattern already proven on GBA; NDS
  save path is a known open question below)." The open question has
  since been CLOSED by Gloamline: its slices 9+ proved the whole NDS
  EEPROM battery-save path headlessly in this repo — including the
  libnds-vs-DeSmuME HOLD-bit protocol fix — so this slice PORTS the
  proven `gl_save_*` shape (fixed 32-byte blob, hash checksum,
  decode-to-zero-on-corrupt, wear-disciplined writes), per the
  session-42 guard recipe ("don't re-derive it").
- authorized by ORDER 005 (`control/inbox.md` @ HEAD `97e0117`):
  "Current tracks keep shipping: next Gloamline + Brineward slices as
  open PRs." Collision scan at claim time: no Brineward claim in
  `control/claims/`, no open Brineward PR (#92/#93 are Tiltstone,
  #85/#87-#90 control/packaging).
- session number 43 claimed (42 = Brineward slice 8, PR #91, merged
  `97e0117`).
- **📊 Model:** Claude 5 family (Fable) — family-level self-report
  from this session's own harness banner, per ORDER 003.
- landing posture: PR opens READY against main; no merge/approve/
  auto-merge calls from this session — the server-side enabler arms
  on green now that required contexts exist (~17:44Z owner click) and
  may auto-land; that is expected.

## Plan (the Gloamline save pattern, ported to the brine)

1. Pure layer + mirror in lockstep: `bw_save_encode/decode/checksum`
   — one 32-byte blob of 8 LE u32 words {magic, version, banked gold,
   packed tiers, charted best band, 2 spares, checksum-of-0..6};
   decode returns 0 (caller keeps the fresh ledger) on ANY bad blob.
2. ARM9 glue: one bounded backup read at power-on restores gold /
   tiers / chart; writes only when the persisted tuple CHANGES (bank,
   port purchase/repair, deeper charted band) — never per frame,
   never on sinking (a sinking changes nothing banked).
3. Proofs: the harness's proven `--battery-in/--battery-out` path;
   new emulator proofs reuse committed stories (the salvage-bank
   story banks 15g = exactly hull II's price) + a mirror-encode
   byte-compare of the battery file + a corrupt-chip boot.
4. Telemetry extends 46 → 50 (words 0-45 frozen, session-42 guard
   recipe; audio slots are NOT save material and stay transient).
5. Heartbeat: ONE dated section appended at the END of
   control/status.md before the flip (coordinator + repo convention).

(Close-out, idea, and previous-session review land at the flip.)
