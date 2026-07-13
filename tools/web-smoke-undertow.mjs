#!/usr/bin/env node
/* Headless smoke test for games/web-undertow/ (browser game, not a ROM).
 *
 * Proves the LOOP end-to-end in a real Chromium: title -> playing on a key
 * event, depth accrues under the fixed-step sim, a no-input run crashes,
 * the same seed + same inputs reproduces the identical crash frame and
 * final score (determinism), steering changes the outcome, and the
 * gameover -> restart transition works. Exits non-zero on any failed
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
const pageURL = pathToFileURL(indexPath).href + `?seed=${SEED}&headless=1`;

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
