#!/usr/bin/env node
/* Browser-level smoke for games/web-tiltstone/ (the render/input shell —
 * the ENGINE proof is games/web-tiltstone/smoke.mjs, pure Node).
 *
 * In a real Chromium: the page loads with ZERO console errors / page errors,
 * the test API is exposed, a keyboard rotation input changes the rendered
 * canvas pixels AND the engine state behind them, and the page state matches
 * a pure-Node engine run of the same seed + inputs (shell adds nothing).
 * Slice 3 (juice): the audio cue log must equal a pure-Node composition of
 * the exact inputs performed, the replay must settle to the plain render of
 * the authoritative state, draw real intermediate frames mid-flight, skip
 * itself under prefers-reduced-motion, and mute must persist while the
 * honest cue log keeps recording.
 * Slice 4 (new cell types): a deep cavern (level 4) generates with ice /
 * locked gems / a one-way grate on the board, renders them without errors,
 * and a real rotation there still matches the pure-Node engine exactly —
 * grates turning, ice sliding — with the cue log staying a pure composition.
 * Slice 5 (level packs): the picker lists the engine's pinned packs, a real
 * click enters stage 1 on the curated seed (grid byte-identical to the
 * pure-Node generator), winning a stage by real key presses advances with N
 * to the next curated seed, and ?pack=&stage= boots straight into a stage.
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

  // --------------------------------------- slice 3: juice (animation + audio)
  const juice = require(path.resolve(here, "..", "games", "web-tiltstone", "juice.js"));

  // The cue log is the shell's honest audio record — recompose it in pure
  // Node from the exact inputs performed above (the shell rule: "rotate" at
  // the press, then the timeline's cues in schedule order, then win/lose;
  // "undo" only when a state actually pops). The page must match EXACTLY.
  const composeCues = (seq) => {
    let st = engine.newGame(SEED, 0);
    const hist = [], log = [];
    for (const it of seq) {
      if (it === "undo") {
        if (!hist.length || st.status === "won") continue;
        st = hist.pop(); log.push("undo"); continue;
      }
      if (st.status !== "playing") continue;
      const prev = st;
      st = engine.rotate(st, it); hist.push(prev);
      log.push("rotate");
      const tl = juice.timeline(engine.resolveTrace(engine.rotateGrid(prev.grid, it)).phases);
      for (const s of tl.steps) for (const c of s.cues) log.push(juice.cueFor(c.name, c.chain).key);
      if (st.status === "won") log.push("win");
      if (st.status === "lost") log.push("lose");
    }
    return log;
  };
  await page.waitForFunction(() => !window.TILTSTONE.isAnimating());
  const expectedLog = composeCues(["cw", "undo", "undo", "cw", "cw", "cw", "undo"]);
  const pageLog = await page.evaluate(() => window.TILTSTONE.getCueLog());
  check("cue log == pure-Node composition of the exact inputs performed",
    JSON.stringify(pageLog) === JSON.stringify(expectedLog),
    `[${pageLog.join(",")}]${JSON.stringify(pageLog) === JSON.stringify(expectedLog) ? "" : ` != expected [${expectedLog.join(",")}]`}`);
  check("cue log carries the cascade (chain-x2 collect keyed) and ends on the win cue",
    pageLog.includes("collect@x2") && pageLog[pageLog.length - 1] === "win",
    `last="${pageLog[pageLog.length - 1]}"`);

  // the replay is cosmetic: once it settles, the canvas IS the plain render
  // of the authoritative state (a forced re-render changes nothing)
  const settled = await page.evaluate(() => document.getElementById("game").toDataURL());
  const rerendered = await page.evaluate(() => { window.TILTSTONE.render(); return document.getElementById("game").toDataURL(); });
  check("animation settles to the true final render (forced re-render is a no-op)",
    settled === rerendered, `dataURL ${settled.length}B, byte-identical`);

  // prefers-reduced-motion: the input applies with NO animation frames, and
  // the cues still land in the log in the same order
  await page.emulateMedia({ reducedMotion: "reduce" });
  await page.evaluate(() => window.TILTSTONE.reset(42, 0));
  const logBeforeRM = await page.evaluate(() => window.TILTSTONE.getCueLog().length);
  await page.keyboard.press("ArrowRight");
  const rm = await page.evaluate(() => ({
    animating: window.TILTSTONE.isAnimating(),
    used: window.TILTSTONE.getState().used,
    log: window.TILTSTONE.getCueLog()
  }));
  const rmExpected = composeCues(["cw"]);
  check("prefers-reduced-motion: input applies instantly with zero animation, cues still logged in order",
    rm.animating === false && rm.used === 1 &&
    JSON.stringify(rm.log.slice(logBeforeRM)) === JSON.stringify(rmExpected),
    `animating=${rm.animating} new cues=[${rm.log.slice(logBeforeRM).join(",")}]`);
  await page.emulateMedia({ reducedMotion: null });

  // mute: toggles + persists (guarded localStorage), and the honest log
  // keeps recording while muted
  await page.click("#btn-mute");
  const mutedNow = await page.evaluate(() => ({
    muted: window.TILTSTONE.isMuted(),
    label: document.getElementById("btn-mute").textContent,
    stored: (() => { try { return window.localStorage.getItem("tiltstone-muted"); } catch { return null; } })(),
    logLen: window.TILTSTONE.getCueLog().length
  }));
  await page.keyboard.press("ArrowRight");
  await page.waitForFunction(() => !window.TILTSTONE.isAnimating());
  const mutedAfter = await page.evaluate(() => ({ logLen: window.TILTSTONE.getCueLog().length }));
  await page.click("#btn-mute"); // leave sound on for the next player
  check("mute toggles + persists, and the cue log keeps recording while muted",
    mutedNow.muted === true && mutedNow.label === "Sound: off" && mutedNow.stored === "1" &&
    mutedAfter.logLen > mutedNow.logLen,
    `label="${mutedNow.label}" stored=${mutedNow.stored} log ${mutedNow.logLen} -> ${mutedAfter.logLen}`);

  // the replay really draws INTERMEDIATE states: a frame sampled mid-flight
  // is neither the pre-input board nor the settled one
  await page.evaluate(() => window.TILTSTONE.reset(42, 0));
  for (const ch of solution.slice(0, -1)) {
    await page.keyboard.press(ch === "R" ? "ArrowRight" : "ArrowLeft");
    await page.waitForFunction(() => !window.TILTSTONE.isAnimating());
  }
  const preShot = await page.evaluate(() => document.getElementById("game").toDataURL());
  await page.keyboard.press(solution[solution.length - 1] === "R" ? "ArrowRight" : "ArrowLeft");
  await page.waitForTimeout(430); // inside the cascade window (sweep 150ms + ~950ms trace)
  const mid = await page.evaluate(() => ({
    shot: document.getElementById("game").toDataURL(),
    animating: window.TILTSTONE.isAnimating()
  }));
  if (shotPath) {
    const midPath = shotPath.replace(/\.png$/, "-mid.png");
    await page.screenshot({ path: midPath });
    console.log(`(mid-animation screenshot: ${midPath})`);
  }
  await page.waitForFunction(() => !window.TILTSTONE.isAnimating());
  const finalShot = await page.evaluate(() => document.getElementById("game").toDataURL());
  check("mid-animation frame differs from both the pre-input and the settled board",
    mid.animating && mid.shot !== preShot && mid.shot !== finalShot,
    `sampled at 430ms, animating=${mid.animating}`);

  // ------------------------------------ slice 4: new cell types (deep cavern)
  await page.evaluate(() => window.TILTSTONE.reset(42, 4));
  const deep = await page.evaluate(() => ({
    grid: window.TILTSTONE.gridString(),
    status: window.TILTSTONE.getState().status,
    level: window.TILTSTONE.getState().levelIndex,
    logLen: window.TILTSTONE.getCueLog().length
  }));
  check("LV5 cavern (level 4) generates with ice, locked gems and a grate on the board",
    deep.status === "playing" && deep.level === 4 &&
    /\*/.test(deep.grid) && /[a-h]/.test(deep.grid) && /[\^>v<]/.test(deep.grid),
    `grid has ${(deep.grid.match(/\*/g) || []).length} ice, ${(deep.grid.match(/[a-h]/g) || []).length} locks, ${(deep.grid.match(/[\^>v<]/g) || []).length} grates`);
  check("deep cavern matches the pure-Node generator byte-for-byte",
    deep.grid === engine.gridString(engine.newGame(42, 4).grid));

  const deepPixBefore = await page.evaluate(() => document.getElementById("game").toDataURL());
  await page.keyboard.press("ArrowRight");
  const deepAfter = await page.evaluate(() => ({
    used: window.TILTSTONE.getState().used,
    grid: window.TILTSTONE.gridString()
  }));
  await page.waitForFunction(() => !window.TILTSTONE.isAnimating());
  const deepPixAfter = await page.evaluate(() => document.getElementById("game").toDataURL());
  const pure4 = engine.rotate(engine.newGame(42, 4), "cw");
  check("a real rotation in the deep cavern == pure-Node engine (grate turned, everything settled)",
    deepAfter.used === 1 && deepAfter.grid === engine.gridString(pure4.grid));
  check("the new cells render (rotation changes the canvas pixels)",
    deepPixBefore !== deepPixAfter);

  const deepTl = juice.timeline(engine.resolveTrace(engine.rotateGrid(engine.newGame(42, 4).grid, "cw")).phases);
  const deepExpected = ["rotate"];
  for (const s of deepTl.steps) for (const c of s.cues) deepExpected.push(juice.cueFor(c.name, c.chain).key);
  if (pure4.status === "won") deepExpected.push("win");
  if (pure4.status === "lost") deepExpected.push("lose");
  const deepLog = await page.evaluate(() => window.TILTSTONE.getCueLog());
  check("deep-cavern cue log tail == pure-Node composition (unlock cue wired through the same table)",
    JSON.stringify(deepLog.slice(deep.logLen)) === JSON.stringify(deepExpected),
    `[${deepLog.slice(deep.logLen).join(",")}]`);

  check("zero console/page errors accumulated through the deep-cavern section",
    consoleErrors.length === 0 && pageErrors.length === 0,
    consoleErrors.concat(pageErrors).join(" | ") || "clean");

  // ------------------------------------- slice 5: level packs (curated seeds)
  const packs = engine.PACKS;
  const deepPack = packs.find(p => p.id === "deep-cuts");

  const pb = await page.evaluate(() => ({
    options: Array.from(document.getElementById("packbox").options).map(o => o.value),
    pack: window.TILTSTONE.getPack()
  }));
  check("pack picker lists the engine's pinned packs (and free play has no active pack)",
    JSON.stringify(pb.options) === JSON.stringify(packs.map(p => p.id)) && pb.pack === null,
    `options=[${pb.options.join(",")}]`);

  await page.selectOption("#packbox", "deep-cuts");   // real UI: select + click
  await page.click("#btn-pack");
  const st1 = await page.evaluate(() => ({
    pack: window.TILTSTONE.getPack(),
    grid: window.TILTSTONE.gridString(),
    seed: window.TILTSTONE.getState().seed,
    level: window.TILTSTONE.getState().levelIndex,
    hud: document.getElementById("hud-pack").textContent
  }));
  check("Play pack (real click) enters stage 1: curated seed at the pack's depth, HUD names it",
    st1.pack && st1.pack.id === "deep-cuts" && st1.pack.stage === 0 &&
    st1.seed === deepPack.entries[0].seed && st1.level === deepPack.levelIndex &&
    st1.hud === `${deepPack.name} 1/${deepPack.entries.length}`,
    `seed=${st1.seed} hud="${st1.hud}"`);
  check("pack stage grid == pure-Node generator on the curated seed, byte-for-byte",
    st1.grid === engine.gridString(engine.newGame(deepPack.entries[0].seed, deepPack.levelIndex).grid));

  // win stage 1 with REAL key presses of the curated level's own solver line
  const lvl1 = engine.generateLevel(deepPack.entries[0].seed, deepPack.levelIndex);
  for (const ch of lvl1.solution) await page.keyboard.press(ch === "R" ? "ArrowRight" : "ArrowLeft");
  await page.waitForFunction(() => !window.TILTSTONE.isAnimating());
  const wonStage = await page.evaluate(() => ({
    status: window.TILTSTONE.getState().status,
    msg: document.getElementById("msg").textContent,
    nextEnabled: !document.getElementById("btn-next").disabled
  }));
  check("stage 1 won by real key presses; the win message points at stage 2",
    wonStage.status === "won" && wonStage.nextEnabled &&
    wonStage.msg.includes(`stage 2/${deepPack.entries.length}`),
    `solution="${lvl1.solution}" msg="${wonStage.msg}"`);

  await page.keyboard.press("n");                     // advance within the pack
  const st2 = await page.evaluate(() => ({
    pack: window.TILTSTONE.getPack(),
    grid: window.TILTSTONE.gridString(),
    seed: window.TILTSTONE.getState().seed
  }));
  check("N advances to stage 2 — the NEXT curated seed, byte-identical to pure Node",
    st2.pack && st2.pack.stage === 1 && st2.seed === deepPack.entries[1].seed &&
    st2.grid === engine.gridString(engine.newGame(deepPack.entries[1].seed, deepPack.levelIndex).grid),
    `seed ${st1.seed} -> ${st2.seed}`);

  // ?pack=&stage= boots straight into a pack stage (fresh page load)
  const granite = packs.find(p => p.id === "granite-gauntlet");
  await page.goto(pathToFileURL(indexPath).href + "?pack=granite-gauntlet&stage=2", { waitUntil: "load" });
  await page.waitForFunction(() => window.TILTSTONE && window.TILTSTONE.getState);
  const boot = await page.evaluate(() => ({
    pack: window.TILTSTONE.getPack(),
    grid: window.TILTSTONE.gridString(),
    hud: document.getElementById("hud-pack").textContent
  }));
  check("?pack=&stage= boots straight into the pack stage (1-based URL, curated seed)",
    boot.pack && boot.pack.id === "granite-gauntlet" && boot.pack.stage === 1 &&
    boot.grid === engine.gridString(engine.newGame(granite.entries[1].seed, granite.levelIndex).grid) &&
    boot.hud === `${granite.name} 2/${granite.entries.length}`,
    `hud="${boot.hud}"`);

  check("zero console/page errors accumulated through the pack section",
    consoleErrors.length === 0 && pageErrors.length === 0,
    consoleErrors.concat(pageErrors).join(" | ") || "clean");

  if (shotPath) { await page.screenshot({ path: shotPath }); console.log(`(screenshot: ${shotPath} — GRANITE GAUNTLET stage 2 via ?pack= boot)`); }
} finally {
  await browser.close();
}

if (failures) { console.error(`BROWSER SMOKE FAIL: ${failures} red`); process.exit(1); }
console.log("BROWSER SMOKE PASS: page shell is a faithful window onto the engine");
