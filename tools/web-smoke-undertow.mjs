#!/usr/bin/env node
/* Headless smoke test for games/web-undertow/ (browser game, not a ROM).
 *
 * Proves the LOOP end-to-end in a real Chromium: title -> playing on a key
 * event, depth accrues under the fixed-step sim, a no-input run crashes,
 * the same seed + same inputs reproduces the identical crash frame and
 * final score (determinism), steering changes the outcome, and the
 * gameover -> restart transition works. Also proves the daily/share cut:
 * share URL pins ?seed=N&depth=M from the finished run, S yields share
 * feedback, dailySeedFor derives YYYYMMDD, ?daily=1 boots on the UTC date
 * (via an injected fake Date), a daily run is identical to the explicit
 * ?seed=YYYYMMDD run, explicit seed beats ?daily=1, and the ?depth= param
 * is render-only (does not perturb the sim). Also proves the cosmetics
 * cut: ?skin=ID selects, C cycles and persists to localStorage, the
 * persisted skin survives a reload, unknown IDs fall back, and — the sim
 * identity proof — a run under a non-default skin crashes on the
 * IDENTICAL frame at the IDENTICAL depth as the default skin for the
 * same seed. Also proves the ghost-replay cut (fresh context = clean
 * storage): a crashed run persists its input timeline as the seed's
 * ghost record; REPLAY FIDELITY — the stored timeline replayed through
 * a fresh sim instance reproduces the original crashFrame and depth
 * exactly; LIVE-RUN IDENTITY — a run with the ghost active is identical
 * to the cleared-storage baseline, and the lockstep ghost lands on its
 * recorded numbers during that run; only the best run per seed is kept;
 * the record survives a reload and re-arms; records are keyed by seed;
 * ?ghost=0 opts out without touching the run. Also proves the oxygen +
 * air-pocket cut (growth cut 4 — a SIM change: the v1.3.0 seed-7
 * no-input fixed point crashFrame=823/194m (wall) became
 * crashFrame=810/191m (out of air); the hold-left wall crash at frame
 * 37 carries verbatim, proving the pocket side-band RNG left the
 * channel layout untouched): the tank is full at run start and drains
 * deterministically; two zero-pocket seeds die of air on the identical
 * frame (air-out is the drain schedule, not the seed); a collected
 * pocket extends the run; a pocket-chasing driver witnesses an in-run
 * refill and far outlives the no-input run on the same seed (the
 * "reason to leave the safe line"), deterministically across two runs;
 * stale v1 ghost records (pre-oxygen sim) are dropped cleanly and a
 * crashed run writes a fresh v2 record. Exits non-zero on any failed
 * assertion; prints one PASS/FAIL line per assertion.
 *
 * Deps (NOT vendored — install next to the script or point NODE_PATH at an
 * external install; do not commit node_modules):
 *     npm i playwright-core        # no browser download needed
 *
 * A Chromium binary must exist locally; set CHROMIUM_BIN to its path
 * (default: /opt/pw-browsers/chromium — a Playwright-provisioned build).
 *
 * Usage (from the repo root):
 *     NODE_PATH=/path/to/node_modules CHROMIUM_BIN=/opt/pw-browsers/chromium \
 *         node tools/web-smoke-undertow.mjs [--shot OUT.png]
 */
import { createRequire } from "node:module";
import { fileURLToPath, pathToFileURL } from "node:url";
import path from "node:path";

const require = createRequire(import.meta.url);
let chromium;
try {
  ({ chromium } = require("playwright-core"));
} catch (e) {
  // createRequire resolves relative to this file; fall back to NODE_PATH dirs.
  const roots = (process.env.NODE_PATH || "").split(path.delimiter).filter(Boolean);
  let loaded = false;
  for (const root of roots) {
    try {
      ({ chromium } = require(path.join(root, "playwright-core")));
      loaded = true;
      break;
    } catch { /* try next */ }
  }
  if (!loaded) {
    console.error("FATAL: playwright-core not resolvable. `npm i playwright-core` next to this script or set NODE_PATH.");
    process.exit(2);
  }
}

const SEED = 7;
const MAX_FRAMES = 36000; // 10 sim-minutes bound on the crash search
const CHUNK = 600;

const here = path.dirname(fileURLToPath(import.meta.url));
const indexPath = path.resolve(here, "..", "games", "web-undertow", "index.html");
const baseURL = pathToFileURL(indexPath).href;
const pageURL = baseURL + `?seed=${SEED}&headless=1`;

// Fake "today" injected into the daily-mode page: 2030-06-07 UTC.
const FAKE_TODAY = { y: 2030, m: 6, d: 7, seed: 20300607 };

const shotIdx = process.argv.indexOf("--shot");
const shotPath = shotIdx !== -1 ? process.argv[shotIdx + 1] : null;

let failures = 0;
function check(name, ok, detail = "") {
  console.log(`${ok ? "PASS" : "FAIL"}: ${name}${detail ? ` — ${detail}` : ""}`);
  if (!ok) failures++;
}

// Run the sim from the current state until gameover (or the frame bound),
// in chunks. Returns {state, frame, score, crashFrame}.
async function runToCrash(page) {
  return await page.evaluate(async ({ max, chunk }) => {
    const U = window.UNDERTOW;
    let last = null;
    let stepped = 0;
    while (stepped < max) {
      last = U.stepFrames(Math.min(chunk, max - stepped));
      stepped += chunk;
      if (last.state === "gameover") break;
    }
    return last;
  }, { max: MAX_FRAMES, chunk: CHUNK });
}

const main = async () => {
  const browser = await chromium.launch({
    executablePath: process.env.CHROMIUM_BIN || "/opt/pw-browsers/chromium",
    headless: true,
    args: ["--no-sandbox"],
  });
  const page = await browser.newPage({ viewport: { width: 520, height: 700 } });
  page.on("pageerror", (e) => { console.error("PAGE ERROR:", e.message); failures++; });
  await page.goto(pageURL);

  // (1) API present, initial state is title
  const boot = await page.evaluate(() => {
    const U = window.UNDERTOW;
    if (!U) return null;
    return { state: U.getState(), seed: U.getSeed(), version: U.version, score: U.getScore() };
  });
  check("UNDERTOW API exposed", !!boot, boot ? `version=${boot.version}` : "window.UNDERTOW missing");
  if (!boot) { await browser.close(); process.exit(1); }
  check("initial state is 'title'", boot.state === "title", `state=${boot.state}, seed=${boot.seed}`);
  check(`seed honored from query (?seed=${SEED})`, boot.seed === SEED, `seed=${boot.seed}`);

  // (2) Space keydown starts the game (dispatched KeyboardEvent, headless)
  await page.evaluate(() => {
    window.dispatchEvent(new KeyboardEvent("keydown", { code: "Space", key: " ", bubbles: true }));
  });
  const afterSpace = await page.evaluate(() => window.UNDERTOW.getState());
  check("Space keydown -> 'playing'", afterSpace === "playing", `state=${afterSpace}`);

  // (3) stepFrames(180) advances depth/score
  const after180 = await page.evaluate(() => {
    const r = window.UNDERTOW.stepFrames(180);
    return { ...r, rows: window.UNDERTOW.getDepthRows() };
  });
  check("stepFrames(180) increases depth/score", after180.score > 0 && after180.rows > 0,
    `score=${after180.score}m rows=${after180.rows} state=${after180.state}`);

  if (shotPath) {
    await page.screenshot({ path: shotPath });
    console.log(`INFO: mid-run screenshot -> ${shotPath}`);
  }

  // (4) no-steering run to gameover (bounded)
  const runA = await runToCrash(page);
  check("no-input run reaches 'gameover' within bound", runA.state === "gameover",
    `state=${runA.state} crashFrame=${runA.crashFrame} finalScore=${runA.score}m`);

  // (5) determinism: reset(SEED), replay identically (same start, no steering)
  await page.evaluate((s) => {
    window.UNDERTOW.reset(s);
    window.dispatchEvent(new KeyboardEvent("keydown", { code: "Space", key: " ", bubbles: true }));
  }, SEED);
  const runB = await runToCrash(page);
  check("determinism: same seed + same input => identical run",
    runB.state === "gameover" && runB.crashFrame === runA.crashFrame && runB.score === runA.score,
    `runA(crashFrame=${runA.crashFrame}, score=${runA.score}m) vs runB(crashFrame=${runB.crashFrame}, score=${runB.score}m)`);

  // (6) input matters: reset(SEED), hold left via setInput
  await page.evaluate((s) => {
    window.UNDERTOW.reset(s);
    window.dispatchEvent(new KeyboardEvent("keydown", { code: "Space", key: " ", bubbles: true }));
    window.UNDERTOW.setInput({ left: true, right: false });
  }, SEED);
  const runC = await runToCrash(page);
  check("steering changes the outcome (hold-left run differs)",
    runC.state === "gameover" && runC.crashFrame !== runA.crashFrame,
    `hold-left crashFrame=${runC.crashFrame} vs no-input crashFrame=${runA.crashFrame}`);

  // (6b) carried v1.3.0 fixed point: the hold-left run dies on the wall at
  // frame 37 / 7m — long before oxygen could matter and without touching a
  // pocket, so this literal carrying verbatim proves the pocket side-band
  // RNG stream left the channel layout and steering physics byte-identical.
  check("carried v1.3.0 fixed point: hold-left seed-7 wall crash at frame 37 / 7m (channel layout untouched)",
    runC.crashFrame === 37 && runC.score === 7 && runC.cause === "wall",
    `crashFrame=${runC.crashFrame} score=${runC.score}m cause=${runC.cause}`);

  // (7) restart loop: after gameover, Space -> playing again, fresh run
  await page.evaluate(() => {
    window.UNDERTOW.setInput({ left: false, right: false });
    window.dispatchEvent(new KeyboardEvent("keydown", { code: "Space", key: " ", bubbles: true }));
  });
  const restart = await page.evaluate(() => ({
    state: window.UNDERTOW.getState(),
    score: window.UNDERTOW.getScore(),
  }));
  check("gameover -> Space restarts a fresh run", restart.state === "playing" && restart.score === 0,
    `state=${restart.state} score=${restart.score}m`);

  // (8) share URL after a crash pins ?seed=N&depth=M from the finished run
  const runD = await runToCrash(page);
  const share = await page.evaluate(() => window.UNDERTOW.getShareURL());
  check("share URL pins ?seed=N&depth=M from the finished run",
    runD.state === "gameover" && runD.score === runA.score
      && share.endsWith(`?seed=${SEED}&depth=${runD.score}`),
    `...${share.slice(share.indexOf("?"))} (score=${runD.score}m, runA=${runA.score}m)`);

  // (9) S on gameover yields share feedback (clipboard confirmation, or the
  // visible link where the clipboard is unavailable — both are the guarded paths)
  await page.evaluate(() => {
    window.dispatchEvent(new KeyboardEvent("keydown", { code: "KeyS", key: "s", bubbles: true }));
  });
  let feedback = "";
  try {
    await page.waitForFunction(() => window.UNDERTOW.getShareFeedback() !== "", null, { timeout: 3000 });
    feedback = await page.evaluate(() => window.UNDERTOW.getShareFeedback());
  } catch { /* feedback stayed empty — the check below fails with detail */ }
  check("S on gameover yields share feedback (copied, or visible link fallback)",
    feedback === "challenge link copied" || feedback === share,
    feedback === "" ? "no feedback within 3s" : `feedback=${feedback.slice(0, 60)}`);

  // (10) pure daily-seed derivation from an arbitrary injected date
  const derived = await page.evaluate(() => window.UNDERTOW.dailySeedFor(new Date(Date.UTC(2031, 0, 5))));
  check("dailySeedFor derives UTC YYYYMMDD (2031-01-05 -> 20310105)",
    derived === 20310105, `derived=${derived}`);

  // (11) ?daily=1 boots with seed = UTC YYYYMMDD — fake Date injected before load
  const dailyPage = await browser.newPage({ viewport: { width: 520, height: 700 } });
  dailyPage.on("pageerror", (e) => { console.error("PAGE ERROR (daily):", e.message); failures++; });
  await dailyPage.addInitScript((f) => {
    const RealDate = Date;
    const FIXED = RealDate.UTC(f.y, f.m - 1, f.d, 12, 34, 56);
    class FakeDate extends RealDate {
      constructor(...args) {
        if (args.length === 0) super(FIXED);
        else super(...args);
      }
      static now() { return FIXED; }
    }
    window.Date = FakeDate;
  }, FAKE_TODAY);
  await dailyPage.goto(baseURL + "?daily=1&headless=1");
  const dailyBoot = await dailyPage.evaluate(() => ({
    seed: window.UNDERTOW.getSeed(), mode: window.UNDERTOW.getMode(),
  }));
  check(`?daily=1 boots with seed = UTC YYYYMMDD (fake today ${FAKE_TODAY.seed})`,
    dailyBoot.seed === FAKE_TODAY.seed && dailyBoot.mode === "daily",
    `seed=${dailyBoot.seed} mode=${dailyBoot.mode}`);

  // (12) the daily run IS the ?seed=YYYYMMDD run — date only feeds seed selection
  await dailyPage.evaluate(() => {
    window.dispatchEvent(new KeyboardEvent("keydown", { code: "Space", key: " ", bubbles: true }));
  });
  const runDaily = await runToCrash(dailyPage);
  await page.goto(baseURL + `?seed=${FAKE_TODAY.seed}&headless=1`);
  await page.evaluate(() => {
    window.dispatchEvent(new KeyboardEvent("keydown", { code: "Space", key: " ", bubbles: true }));
  });
  const runRef = await runToCrash(page);
  check("daily run identical to explicit ?seed=YYYYMMDD run (same crash frame + score)",
    runDaily.state === "gameover" && runRef.state === "gameover"
      && runDaily.crashFrame === runRef.crashFrame && runDaily.score === runRef.score,
    `daily(crashFrame=${runDaily.crashFrame}, score=${runDaily.score}m) vs seeded(crashFrame=${runRef.crashFrame}, score=${runRef.score}m)`);

  // (13) daily share link pins the explicit seed, never ?daily=1
  const dailyShare = await dailyPage.evaluate(() => window.UNDERTOW.getShareURL());
  check("daily share link pins ?seed=YYYYMMDD (no daily param)",
    dailyShare.endsWith(`?seed=${FAKE_TODAY.seed}&depth=${runDaily.score}`) && !dailyShare.includes("daily"),
    `...${dailyShare.slice(dailyShare.indexOf("?"))}`);

  // (14) explicit ?seed=N beats ?daily=1 (a shared link replays on any day)
  await dailyPage.goto(baseURL + `?seed=${SEED}&daily=1&headless=1`);
  const seededBoot = await dailyPage.evaluate(() => ({
    seed: window.UNDERTOW.getSeed(), mode: window.UNDERTOW.getMode(),
  }));
  check("explicit ?seed=N wins over ?daily=1",
    seededBoot.seed === SEED && seededBoot.mode === "seeded",
    `seed=${seededBoot.seed} mode=${seededBoot.mode}`);
  await dailyPage.close();

  // (15) ?depth=M is render-only: challenge shown, sim untouched
  await page.goto(baseURL + `?seed=${SEED}&depth=123&headless=1`);
  const challenge = await page.evaluate(() => window.UNDERTOW.getChallengeDepth());
  await page.evaluate(() => {
    window.dispatchEvent(new KeyboardEvent("keydown", { code: "Space", key: " ", bubbles: true }));
  });
  const runE = await runToCrash(page);
  check("?depth=123 is render-only (challenge shown, run identical to bare seed)",
    challenge === 123 && runE.state === "gameover"
      && runE.crashFrame === runA.crashFrame && runE.score === runA.score,
    `challengeDepth=${challenge} crashFrame=${runE.crashFrame} (bare=${runA.crashFrame}) score=${runE.score}m (bare=${runA.score}m)`);

  // (16) ?skin=ember selects the ember skin (render-side cosmetic)
  await page.goto(baseURL + `?seed=${SEED}&skin=ember&headless=1`);
  const emberBoot = await page.evaluate(() => ({
    skin: window.UNDERTOW.getSkin(), ids: window.UNDERTOW.getSkinIds(),
  }));
  check("?skin=ember selects the ember skin",
    emberBoot.skin.id === "ember" && emberBoot.ids.includes("classic"),
    `skin=${emberBoot.skin.id} trail=${emberBoot.skin.trailStyle} ids=[${emberBoot.ids.join(",")}]`);

  // (17) SIM IDENTITY ACROSS SKINS: the same seed under a non-default skin
  // crashes on the identical frame at the identical depth as the default
  // skin (runA). Cosmetics are pure render — this is the zero-sim-risk proof.
  await page.evaluate(() => {
    window.dispatchEvent(new KeyboardEvent("keydown", { code: "Space", key: " ", bubbles: true }));
  });
  const runSkin = await runToCrash(page);
  check("sim identity across skins: ember run == default-skin run (same crashFrame + depth)",
    runSkin.state === "gameover" && runSkin.crashFrame === runA.crashFrame && runSkin.score === runA.score,
    `ember(crashFrame=${runSkin.crashFrame}, score=${runSkin.score}m) vs default(crashFrame=${runA.crashFrame}, score=${runA.score}m)`);

  // (18) C on gameover cycles the skin and persists it to localStorage
  const afterCycle = await page.evaluate(() => {
    window.dispatchEvent(new KeyboardEvent("keydown", { code: "KeyC", key: "c", bubbles: true }));
    let stored = null;
    try { stored = window.localStorage.getItem("undertow.skin"); } catch { /* guarded */ }
    return { skin: window.UNDERTOW.getSkin(), stored };
  });
  check("C cycles the skin (ember -> ghost) and persists it",
    afterCycle.skin.id === "ghost" && afterCycle.stored === "ghost",
    `skin=${afterCycle.skin.id} localStorage=${afterCycle.stored}`);

  // (19) persisted skin is restored on a reload without ?skin=
  await page.goto(baseURL + `?seed=${SEED}&headless=1`);
  const persisted = await page.evaluate(() => window.UNDERTOW.getSkin());
  check("persisted skin restored on reload without ?skin=",
    persisted.id === "ghost", `skin=${persisted.id}`);

  // (20) unknown ?skin= falls back (to the persisted choice here, never crashes)
  await page.goto(baseURL + `?seed=${SEED}&skin=nope&headless=1`);
  const fallback = await page.evaluate(() => window.UNDERTOW.getSkin());
  check("unknown ?skin=nope falls back to the persisted skin",
    fallback.id === "ghost", `skin=${fallback.id}`);

  // ---- ghost replays (growth cut 3) -----------------------------------------
  // browser.newPage() = a fresh context = EMPTY localStorage: the ghost
  // assertions start from a clean slate, so runG1 below is the
  // cleared-storage (ghost-off) baseline for the identity proof.
  const gpage = await browser.newPage({ viewport: { width: 520, height: 700 } });
  gpage.on("pageerror", (e) => { console.error("PAGE ERROR (ghost):", e.message); failures++; });
  await gpage.goto(baseURL + `?seed=${SEED}&headless=1`);

  // (21) clean slate: no stored ghost, none active
  const gBoot = await gpage.evaluate(() => window.UNDERTOW.getGhostInfo());
  check("clean storage: no stored ghost record, none active",
    gBoot.enabled === true && gBoot.stored === null && gBoot.active === false,
    JSON.stringify(gBoot));

  // (22) a crashed run records + persists its input timeline as the seed's ghost
  await gpage.evaluate(() => {
    window.dispatchEvent(new KeyboardEvent("keydown", { code: "Space", key: " ", bubbles: true }));
  });
  const runG1 = await runToCrash(gpage);
  const gAfter = await gpage.evaluate(() => {
    let raw = null;
    try { raw = window.localStorage.getItem("undertow.ghost." + window.UNDERTOW.getSeed()); } catch { /* guarded */ }
    return { info: window.UNDERTOW.getGhostInfo(), rawLen: raw ? raw.length : 0 };
  });
  check("crashed run persists a best-run ghost record (depth + crashFrame + input log)",
    runG1.state === "gameover" && runG1.crashFrame === runA.crashFrame && runG1.score === runA.score
      && !!gAfter.info.stored && gAfter.info.stored.depth === runG1.score
      && gAfter.info.stored.crashFrame === runG1.crashFrame && gAfter.rawLen > 0,
    `stored=${JSON.stringify(gAfter.info.stored)} run(crashFrame=${runG1.crashFrame}, score=${runG1.score}m) rawBytes=${gAfter.rawLen}`);

  // (23) REPLAY FIDELITY: the stored input timeline, replayed through a fresh
  // sim instance (no freeze guard), reproduces the original run's crash frame
  // and depth EXACTLY — the determinism selling point.
  const fidelity = await gpage.evaluate(() => window.UNDERTOW.verifyGhost());
  check("replay fidelity: stored timeline reproduces the run exactly (crashFrame + depth)",
    !!fidelity && fidelity.crashed
      && fidelity.frame === runG1.crashFrame && fidelity.depth === runG1.score
      && fidelity.frame === fidelity.recCrashFrame && fidelity.depth === fidelity.recDepth,
    fidelity
      ? `replay(crashFrame=${fidelity.frame}, depth=${fidelity.depth}m) vs recorded(crashFrame=${fidelity.recCrashFrame}, depth=${fidelity.recDepth}m)`
      : "verifyGhost returned null");

  // (24) LIVE-RUN IDENTITY with the ghost active: restart the same seed — the
  // ghost dives alongside — and the live run's crashFrame/depth are IDENTICAL
  // to the cleared-storage baseline (runG1).
  await gpage.evaluate(() => {
    window.dispatchEvent(new KeyboardEvent("keydown", { code: "Space", key: " ", bubbles: true }));
  });
  const gActive = await gpage.evaluate(() => window.UNDERTOW.getGhostInfo().active);
  const runG2 = await runToCrash(gpage);
  check("live-run identity: ghost active vs cleared storage — same crashFrame + depth",
    gActive === true && runG2.state === "gameover"
      && runG2.crashFrame === runG1.crashFrame && runG2.score === runG1.score,
    `ghostActive=${gActive} withGhost(crashFrame=${runG2.crashFrame}, score=${runG2.score}m) vs baseline(crashFrame=${runG1.crashFrame}, score=${runG1.score}m)`);

  // (25) the lockstep ghost itself landed exactly on its recorded numbers
  // during that live run (in-run replay, not just the offline probe)
  const ghostEnd = await gpage.evaluate(() => window.UNDERTOW.getGhostRun());
  check("lockstep ghost lands on its recorded crashFrame + depth during the live run",
    !!ghostEnd && ghostEnd.crashed
      && ghostEnd.frame === ghostEnd.recCrashFrame && ghostEnd.depth === ghostEnd.recDepth,
    ghostEnd
      ? `ghost(crashFrame=${ghostEnd.frame}, depth=${ghostEnd.depth}m) vs recorded(crashFrame=${ghostEnd.recCrashFrame}, depth=${ghostEnd.recDepth}m)`
      : "no ghost run state");

  // (26) best run per seed is the keeper: a different (hold-left) run only
  // replaces the record if it went deeper
  await gpage.evaluate((s) => {
    window.UNDERTOW.reset(s);
    window.dispatchEvent(new KeyboardEvent("keydown", { code: "Space", key: " ", bubbles: true }));
    window.UNDERTOW.setInput({ left: true, right: false });
  }, SEED);
  const runG3 = await runToCrash(gpage);
  await gpage.evaluate(() => window.UNDERTOW.setInput({ left: false, right: false }));
  const bestStored = await gpage.evaluate(() => window.UNDERTOW.getGhostInfo().stored);
  const expectBest = Math.max(runG1.score, runG3.score);
  check("best run per seed is the keeper (shallower run never overwrites)",
    runG3.state === "gameover" && !!bestStored && bestStored.depth === expectBest,
    `stored=${bestStored && bestStored.depth}m runs: no-input ${runG1.score}m vs hold-left ${runG3.score}m`);

  // (27) persistence across reload: the record survives and re-arms the ghost
  await gpage.goto(baseURL + `?seed=${SEED}&headless=1`);
  const reloaded = await gpage.evaluate(() => {
    const before = window.UNDERTOW.getGhostInfo();
    window.dispatchEvent(new KeyboardEvent("keydown", { code: "Space", key: " ", bubbles: true }));
    return { stored: before.stored, activeAfterStart: window.UNDERTOW.getGhostInfo().active };
  });
  check("ghost record survives a reload and re-arms on the next dive",
    !!reloaded.stored && reloaded.stored.depth === expectBest && reloaded.activeAfterStart === true,
    `stored=${JSON.stringify(reloaded.stored)} activeAfterStart=${reloaded.activeAfterStart}`);

  // (28) records are keyed by seed: another seed has no ghost
  await gpage.goto(baseURL + "?seed=12345&headless=1");
  const otherSeed = await gpage.evaluate(() => window.UNDERTOW.getGhostInfo());
  check("ghost records are keyed by seed (other seed: none stored, none active)",
    otherSeed.stored === null && otherSeed.active === false,
    JSON.stringify(otherSeed));

  // (29) ?ghost=0 opts out (record kept, no ghost spawned) and the run is
  // unchanged — the switch is render-side only
  await gpage.goto(baseURL + `?seed=${SEED}&ghost=0&headless=1`);
  const optedOut = await gpage.evaluate(() => {
    window.dispatchEvent(new KeyboardEvent("keydown", { code: "Space", key: " ", bubbles: true }));
    return window.UNDERTOW.getGhostInfo();
  });
  const runG4 = await runToCrash(gpage);
  check("?ghost=0 opts out (record kept, no ghost) and the run is unchanged",
    optedOut.enabled === false && optedOut.stored !== null && optedOut.active === false
      && runG4.state === "gameover"
      && runG4.crashFrame === runG1.crashFrame && runG4.score === runG1.score,
    `enabled=${optedOut.enabled} stored=${!!optedOut.stored} active=${optedOut.active} run(crashFrame=${runG4.crashFrame}, score=${runG4.score}m) vs baseline(${runG1.crashFrame}, ${runG1.score}m)`);
  await gpage.close();

  // ---- oxygen + air pockets (growth cut 4) -----------------------------------
  // A SIM change: run outcomes for a given seed legitimately moved. The
  // no-input seed-7 baseline (runA above) shifted from the v1.3.0 fixed
  // point crashFrame=823 / 194m (wall) to crashFrame=810 / 191m (out of
  // air, one accidental pickup on the way down). These assertions pin the
  // oxygen mechanics themselves. Fresh context = clean storage.
  const AIR_SEED_A = 4;  // dev-verified: no-input run collects zero pockets
  const AIR_SEED_B = 8;  // dev-verified: no-input run collects zero pockets
  const CHASE_SEED = 3;  // dev-verified: pocket-rich channel for the chaser

  // no-input run of a seed; returns crash numbers + end-of-run oxygen state
  async function noInputRun(pg, s) {
    await pg.evaluate((sd) => {
      window.UNDERTOW.reset(sd);
      window.dispatchEvent(new KeyboardEvent("keydown", { code: "Space", key: " ", bubbles: true }));
    }, s);
    const r = await runToCrash(pg);
    const o = await pg.evaluate(() => window.UNDERTOW.getOxygen());
    return { ...r, pockets: o.pockets, oxygen: o.oxygen };
  }

  // greedy pocket-chaser: each frame steer toward the nearest untaken pocket
  // below the diver (via the getPocketProbe test hook). Fully deterministic:
  // it reads only sim state, and the sim is seeded.
  async function chaseRun(pg, s) {
    return await pg.evaluate(({ sd, max }) => {
      const U = window.UNDERTOW;
      U.reset(sd);
      window.dispatchEvent(new KeyboardEvent("keydown", { code: "Space", key: " ", bubbles: true }));
      let refill = null;
      let prev = U.getOxygen().oxygen;
      let last = null;
      for (let f = 0; f < max; f++) {
        const p = U.getPocketProbe();
        if (p) U.setInput({ left: p.x < p.diverX - 2, right: p.x > p.diverX + 2 });
        else U.setInput({ left: false, right: false });
        last = U.stepFrames(1);
        const o = U.getOxygen();
        if (refill === null && o.oxygen > prev) refill = { frame: last.frame, from: prev, to: o.oxygen };
        prev = o.oxygen;
        if (last.state === "gameover") break;
      }
      U.setInput({ left: false, right: false });
      const o = U.getOxygen();
      return { ...last, pockets: o.pockets, oxygen: o.oxygen, refill };
    }, { sd: s, max: MAX_FRAMES });
  }

  const opage = await browser.newPage({ viewport: { width: 520, height: 700 } });
  opage.on("pageerror", (e) => { console.error("PAGE ERROR (oxygen):", e.message); failures++; });
  await opage.goto(baseURL + `?seed=${SEED}&headless=1`);

  // (30) tank full at run start, drains as you dive, deterministically
  const oxyDep = await opage.evaluate(() => {
    const U = window.UNDERTOW;
    window.dispatchEvent(new KeyboardEvent("keydown", { code: "Space", key: " ", bubbles: true }));
    const start = U.getOxygen();
    U.stepFrames(240);
    const a = U.getOxygen().oxygen;
    U.reset(U.getSeed());
    window.dispatchEvent(new KeyboardEvent("keydown", { code: "Space", key: " ", bubbles: true }));
    U.stepFrames(240);
    const b = U.getOxygen().oxygen;
    return { start: start.oxygen, max: start.max, a, b };
  });
  check("oxygen: tank full at run start, drains as you dive, deterministic at frame 240",
    oxyDep.start === oxyDep.max && oxyDep.a > 0 && oxyDep.a < oxyDep.max && oxyDep.a === oxyDep.b,
    `start=${oxyDep.start}/${oxyDep.max} frame240=${oxyDep.a} rerun=${oxyDep.b}`);

  // (31) the seed-7 no-input run now ends OUT OF AIR at the runA numbers —
  // the sim-change witness (old fixed point: 823 / 194m, wall)
  const oxyRun = await runToCrash(opage);
  const oxyEnd = await opage.evaluate(() => ({
    o: window.UNDERTOW.getOxygen(), cause: window.UNDERTOW.getCrashCause(),
  }));
  check("oxygen-out ends the run: seed-7 no-input run dies of air with an empty tank (new baseline)",
    oxyRun.state === "gameover" && oxyRun.cause === "air" && oxyEnd.cause === "air"
      && oxyEnd.o.oxygen === 0
      && oxyRun.crashFrame === runA.crashFrame && oxyRun.score === runA.score,
    `crashFrame=${oxyRun.crashFrame} score=${oxyRun.score}m cause=${oxyEnd.cause} oxygen=${oxyEnd.o.oxygen} pockets=${oxyEnd.o.pockets} (v1.3.0 fixed point was 823 / 194m, wall)`);

  // (32) zero-pocket air-out is the drain schedule, not the seed: the descent
  // (and so the drain) is seed-independent, so two seeds whose no-input runs
  // touch no pocket die of air on the IDENTICAL frame at the identical depth
  const airA = await noInputRun(opage, AIR_SEED_A);
  const airB = await noInputRun(opage, AIR_SEED_B);
  check("zero-pocket seeds die of air on the identical frame (drain schedule is seed-independent)",
    airA.cause === "air" && airB.cause === "air"
      && airA.pockets === 0 && airB.pockets === 0
      && airA.crashFrame === airB.crashFrame && airA.score === airB.score,
    `seed${AIR_SEED_A}(crashFrame=${airA.crashFrame}, ${airA.score}m, pockets=${airA.pockets}) vs seed${AIR_SEED_B}(crashFrame=${airB.crashFrame}, ${airB.score}m, pockets=${airB.pockets})`);

  // (33) a collected pocket extends the run: seed-7's no-input run drifts
  // through one pocket on the way down and outlives the zero-pocket air-out
  check("a collected pocket extends the run (seed-7's accidental pickup outlives the zero-pocket air-out frame)",
    oxyEnd.o.pockets >= 1 && oxyRun.crashFrame > airA.crashFrame,
    `seed-7 pockets=${oxyEnd.o.pockets} crashFrame=${oxyRun.crashFrame} vs zero-pocket air-out=${airA.crashFrame}`);

  // (34) pockets refill the tank and are the reason to leave the safe line:
  // a greedy pocket-chaser witnesses an in-run refill and far outlives the
  // same seed's no-input run
  const chaseBase = await noInputRun(opage, CHASE_SEED);
  const chase1 = await chaseRun(opage, CHASE_SEED);
  check("air pockets refill the tank (in-run oxygen increase witnessed by the pocket-chaser)",
    chase1.pockets > 0 && chase1.refill !== null && chase1.refill.to > chase1.refill.from,
    chase1.refill
      ? `pockets=${chase1.pockets} first refill @frame ${chase1.refill.frame}: ${chase1.refill.from.toFixed(2)} -> ${chase1.refill.to.toFixed(2)}`
      : `pockets=${chase1.pockets} no refill witnessed`);
  check("pockets are a reason to leave the safe line (chaser far outlives the no-input run, same seed)",
    chase1.state === "gameover" && chaseBase.state === "gameover"
      && chase1.score > chaseBase.score && chase1.crashFrame > chaseBase.crashFrame,
    `chaser(crashFrame=${chase1.crashFrame}, ${chase1.score}m, pockets=${chase1.pockets}) vs no-input(crashFrame=${chaseBase.crashFrame}, ${chaseBase.score}m, pockets=${chaseBase.pockets})`);

  // (35) the driven run is deterministic too: run the chaser twice —
  // identical crash frame, depth, pocket count and final oxygen
  const chase2 = await chaseRun(opage, CHASE_SEED);
  check("pocket-chaser determinism: two driven runs are identical (crashFrame, depth, pockets, oxygen)",
    chase2.crashFrame === chase1.crashFrame && chase2.score === chase1.score
      && chase2.pockets === chase1.pockets && chase2.oxygen === chase1.oxygen,
    `run1(crashFrame=${chase1.crashFrame}, ${chase1.score}m, pockets=${chase1.pockets}) vs run2(crashFrame=${chase2.crashFrame}, ${chase2.score}m, pockets=${chase2.pockets})`);
  await opage.close();

  // (36) stale v1 ghost records (recorded under the oxygen-free sim) are
  // dropped cleanly on load — their depth/crashFrame no longer mean anything
  const vpage = await browser.newPage({ viewport: { width: 520, height: 700 } });
  vpage.on("pageerror", (e) => { console.error("PAGE ERROR (ghost-v2):", e.message); failures++; });
  await vpage.goto(baseURL + `?seed=${SEED}&headless=1`);
  const staleSetup = await vpage.evaluate(() => {
    try {
      window.localStorage.setItem("undertow.ghost." + window.UNDERTOW.getSeed(),
        JSON.stringify({ v: 1, depth: 999, crashFrame: 9999, log: "9999n" }));
      return true;
    } catch { return false; }
  });
  await vpage.reload();
  const staleInfo = await vpage.evaluate(() => window.UNDERTOW.getGhostInfo());
  check("stale v1 ghost record (pre-oxygen sim) is dropped cleanly on load (no ghost, no error)",
    staleSetup === true && staleInfo.stored === null && staleInfo.active === false,
    `injected v1 record; after reload stored=${JSON.stringify(staleInfo.stored)} active=${staleInfo.active}`);

  // (37) the next crashed run replaces the stale record with a fresh v2 ghost
  await vpage.evaluate(() => {
    window.dispatchEvent(new KeyboardEvent("keydown", { code: "Space", key: " ", bubbles: true }));
  });
  const runV = await runToCrash(vpage);
  const v2rec = await vpage.evaluate(() => {
    try { return JSON.parse(window.localStorage.getItem("undertow.ghost." + window.UNDERTOW.getSeed())); }
    catch { return null; }
  });
  check("a crashed run under the oxygen sim writes a fresh v2 ghost record over the stale one",
    runV.state === "gameover" && !!v2rec && v2rec.v === 2
      && v2rec.depth === runV.score && v2rec.crashFrame === runV.crashFrame,
    `record v=${v2rec && v2rec.v} depth=${v2rec && v2rec.depth} crashFrame=${v2rec && v2rec.crashFrame} run(crashFrame=${runV.crashFrame}, ${runV.score}m)`);
  await vpage.close();

  await browser.close();

  console.log(failures === 0
    ? `SMOKE PASS: all assertions green (seed=${SEED}, crashFrame=${runA.crashFrame}, finalScore=${runA.score}m)`
    : `SMOKE FAIL: ${failures} assertion(s) failed`);
  process.exit(failures === 0 ? 0 : 1);
};

main().catch((e) => {
  console.error("FATAL:", e);
  process.exit(2);
});
