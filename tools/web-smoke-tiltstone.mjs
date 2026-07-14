#!/usr/bin/env node
/* Browser-level smoke for games/web-tiltstone/ (the render/input shell —
 * the ENGINE proof is games/web-tiltstone/smoke.mjs, pure Node).
 *
 * In a real Chromium: the page loads with ZERO console errors / page errors,
 * the test API is exposed, a keyboard rotation input changes the rendered
 * canvas pixels AND the engine state behind them, and the page state matches
 * a pure-Node engine run of the same seed + inputs (shell adds nothing).
 *
 * Deps (NOT vendored — do not commit node_modules): playwright or
 * playwright-core resolvable via NODE_PATH; a local Chromium binary,
 * default /opt/pw-browsers/chromium, override with CHROMIUM_BIN.
 *
 * Usage (from the repo root):
 *   NODE_PATH=/opt/node22/lib/node_modules CHROMIUM_BIN=/opt/pw-browsers/chromium \
 *     node tools/web-smoke-tiltstone.mjs [--shot OUT.png]
 */
import { createRequire } from "node:module";
import { fileURLToPath, pathToFileURL } from "node:url";
import path from "node:path";

const require = createRequire(import.meta.url);
let chromium = null;
for (const name of ["playwright", "playwright-core"]) {
  try { ({ chromium } = require(name)); break; } catch { /* try next */ }
}
if (!chromium) {
  const roots = (process.env.NODE_PATH || "").split(path.delimiter).filter(Boolean);
  outer: for (const root of roots) {
    for (const name of ["playwright", "playwright-core"]) {
      try { ({ chromium } = require(path.join(root, name))); break outer; } catch { /* next */ }
    }
  }
}
if (!chromium) {
  console.error("FATAL: playwright(-core) not resolvable; set NODE_PATH at an install.");
  process.exit(2);
}

const SEED = 42;
const here = path.dirname(fileURLToPath(import.meta.url));
const indexPath = path.resolve(here, "..", "games", "web-tiltstone", "index.html");
const pageURL = pathToFileURL(indexPath).href + `?seed=${SEED}`;
const shotIdx = process.argv.indexOf("--shot");
const shotPath = shotIdx !== -1 ? process.argv[shotIdx + 1] : null;

let failures = 0;
function check(name, ok, detail = "") {
  console.log(`${ok ? "PASS" : "FAIL"}: ${name}${detail ? ` — ${detail}` : ""}`);
  if (!ok) failures++;
}

const browser = await chromium.launch({
  executablePath: process.env.CHROMIUM_BIN || "/opt/pw-browsers/chromium",
  args: ["--no-sandbox"]
});
try {
  const page = await browser.newPage();
  const consoleErrors = [], pageErrors = [];
  page.on("console", m => { if (m.type() === "error") consoleErrors.push(m.text()); });
  page.on("pageerror", e => pageErrors.push(String(e)));

  await page.goto(pageURL, { waitUntil: "load" });
  await page.waitForFunction(() => window.TILTSTONE && window.TILTSTONE.getState);

  check("page loads with zero console/page errors",
    consoleErrors.length === 0 && pageErrors.length === 0,
    consoleErrors.concat(pageErrors).join(" | ") || "clean");

  const init = await page.evaluate(() => {
    const s = window.TILTSTONE.getState();
    return { seed: s.seed, status: s.status, used: s.used, collected: s.collected,
             quota: s.quota, budget: s.budget, grid: window.TILTSTONE.gridString() };
  });
  check("test API exposed with the URL seed", init.seed === SEED && init.status === "playing",
    `seed=${init.seed} status=${init.status} quota=${init.quota} budget=${init.budget}`);

  const pixelsBefore = await page.evaluate(() =>
    document.getElementById("game").toDataURL());

  await page.keyboard.press("ArrowRight"); // one CW rotation
  const after = await page.evaluate(() => {
    const s = window.TILTSTONE.getState();
    return { used: s.used, grid: window.TILTSTONE.gridString(),
             turnsHud: document.getElementById("hud-turns").textContent };
  });
  const pixelsAfter = await page.evaluate(() =>
    document.getElementById("game").toDataURL());

  check("ArrowRight rotates: engine state advanced (used 0 -> 1)",
    init.used === 0 && after.used === 1, `HUD says "${after.turnsHud}"`);
  check("rotation changes the rendered canvas pixels",
    pixelsBefore !== pixelsAfter,
    `dataURL ${pixelsBefore.length}B -> ${pixelsAfter.length}B, differs`);
  check("rotation changes the grid", init.grid !== after.grid);

  // Shell adds nothing: page grid after 1 CW == pure engine run of same seed
  const engine = require(path.resolve(here, "..", "games", "web-tiltstone", "engine.js"));
  const pure = engine.rotate(engine.newGame(SEED, 0), "cw");
  check("page state == pure-Node engine state (same seed, same input)",
    after.grid === engine.gridString(pure.grid) && after.used === pure.used);

  // ------------------------------------------- slice 2: par HUD + undo + grade
  const par = engine.par(engine.newGame(SEED, 0).level);
  const parHud = await page.evaluate(() => document.getElementById("hud-par").textContent);
  check("HUD shows the engine's par", parHud === `PAR ${par}`, `"${parHud}" (engine par=${par})`);

  // U takes back the rotation: grid returns byte-identical to the initial pin
  await page.keyboard.press("u");
  const undone = await page.evaluate(() => ({
    used: window.TILTSTONE.getState().used,
    grid: window.TILTSTONE.gridString(),
    undos: window.TILTSTONE.getUndos(),
    histLen: window.TILTSTONE.getHistoryLength()
  }));
  check("U undoes the rotation (used 1 -> 0, grid byte-identical to initial)",
    undone.used === 0 && undone.grid === init.grid && undone.undos === 1 && undone.histLen === 0,
    `used=${undone.used} undos=${undone.undos} histLen=${undone.histLen}`);

  // undo with an empty history is a no-op
  await page.keyboard.press("u");
  const noop = await page.evaluate(() => ({
    used: window.TILTSTONE.getState().used, undos: window.TILTSTONE.getUndos()
  }));
  check("undo on empty history is a no-op (undo count unchanged)",
    noop.used === 0 && noop.undos === 1, `used=${noop.used} undos=${noop.undos}`);

  // play the solver line with REAL key presses (solution for seed 42 is all-R)
  const solution = engine.newGame(SEED, 0).level.solution;
  for (const ch of solution) await page.keyboard.press(ch === "R" ? "ArrowRight" : "ArrowLeft");
  const won = await page.evaluate(() => ({
    status: window.TILTSTONE.getState().status,
    used: window.TILTSTONE.getState().used,
    msg: document.getElementById("msg").textContent,
    nextEnabled: !document.getElementById("btn-next").disabled,
    undoDisabled: document.getElementById("btn-undo").disabled
  }));
  check("solver line by real key presses wins the level",
    won.status === "won" && won.used === solution.length && won.nextEnabled,
    `solution="${solution}" -> status=${won.status} used=${won.used}`);
  check("win message carries the grade (par turns after the undo -> PERFECT + undo count)",
    won.msg.includes("PERFECT") && won.msg.includes(`PAR ${par}`) && won.msg.includes("1 undo"),
    `msg="${won.msg}"`);

  // a WON card is frozen: U is a no-op and the button is disabled
  await page.keyboard.press("u");
  const frozen = await page.evaluate(() => ({
    status: window.TILTSTONE.getState().status, used: window.TILTSTONE.getState().used
  }));
  check("undo after a win is a no-op (won card frozen)",
    won.undoDisabled && frozen.status === "won" && frozen.used === solution.length,
    `status=${frozen.status} used=${frozen.used}`);

  if (shotPath) { await page.screenshot({ path: shotPath }); console.log(`(screenshot: ${shotPath})`); }
} finally {
  await browser.close();
}

if (failures) { console.error(`BROWSER SMOKE FAIL: ${failures} red`); process.exit(1); }
console.log("BROWSER SMOKE PASS: page shell is a faithful window onto the engine");
