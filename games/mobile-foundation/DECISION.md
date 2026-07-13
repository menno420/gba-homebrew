# Mobile foundation — framework decision

> **Status:** `decided` — 2026-07-13 (mobile-foundation session, owner night
> order). One page: options weighed, the pick, why, evidence.

## Options weighed

| Option | What it needs | What this container has | Verdict |
|---|---|---|---|
| **Static PWA** (plain HTML/ES modules + manifest + service worker) | A text editor, a browser to prove it in | Everything: python3 http.server, headless Chromium at `/opt/pw-browsers` | **PICK** |
| **Capacitor-style shell** (webview wrapped in a native project) | npm dependency tree + Android SDK to produce the shell | npm reachable, but the shell build still dead-ends at the missing SDK; adds a dependency tree for zero playable gain over the PWA it wraps | No — deferred until store packaging is real |
| **Native Android** (Kotlin/Java + Gradle + SDK) | JDK, Gradle, **Android SDK** (`sdkmanager`, platforms, build-tools) | JDK + Gradle present, **SDK absent** — probe 2026-07-13: `which sdkmanager` empty; installing is a multi-hundred-MB owner-approved download | No — walled here |
| **Godot export** | Godot editor + export templates + (for Android) the same SDK | **Absent** — probe 2026-07-13, verbatim: `/bin/bash: line 1: godot: command not found` (exit 127) | No — walled here |

Probe evidence is on the ledger: `docs/PLATFORM-LIMITS.md` → "Native Android
SDK + Godot are absent from this container" (2026-07-13T00:51:30Z, verbatim
output, one probe per the discovery rule).

## The pick: npm-free static PWA

- **Provable end-to-end in this container.** Serve with
  `python3 -m http.server`, drive with the pre-installed headless Chromium,
  assert game state headlessly. Every other option's proof loop dies at a
  missing toolchain or an owner download.
- **Zero dependency surface.** Plain ES modules, no bundler, no
  `node_modules`, nothing vendored — keeps the PUBLIC repo clean and the
  supply chain empty. A build pipeline that is just "serve these files" is
  a pipeline that cannot rot.
- **Genuinely mobile.** Installable (manifest + service worker = add to
  home screen, offline via cache-first SW), fullscreen standalone display,
  touch-first input. This is a real phone game surface, not a desktop demo.
- **Store path stays open.** A compliant PWA is the exact input
  Bubblewrap/TWA consumes to mint a Play Store AAB — the PWA is the
  foundation, packaging is a bolt-on later (owner action: store account,
  signing keys, SDK download). See `CONCEPT.md` § packaging.

## What would change this decision

Owner provisions the Android SDK or a Godot binary in the environment
(retire the wall entry with a fresh probe first), or the first game outgrows
canvas-2D performance on real phones — re-evaluate then, with evidence.
