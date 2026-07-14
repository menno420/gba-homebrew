#!/usr/bin/env node
// Headless GAME proof for games/mobile-foundation (Drift Garden playable slice).
//
// Asserts, against the real served page in headless Chromium:
//   1. BOOT        — ?seed=42 boots into phase 'playing' with seed 42
//   2. TOUCH       — a constructed TouchEvent dispatched on the canvas
//                    (the real touchstart handler) counts a tap and plants
//   3. WIN         — scripted round via window.__game.tapAt/step: plant,
//                    mature, cross-pollinate (tier-2 appears), harvest to
//                    essence >= quota before dusk -> phase 'won'
//   4. LOSE        — same seed, no harvesting, step past dusk -> 'lost',
//                    with wisps spawned and motes eaten along the way
//   5. DETERMINISM — two fresh loads with ?seed=7 + identical scripted
//                    input give byte-identical snapshot() JSON; seed 8 differs
//   6. WEATHER     — daily seeded weather: weatherFor(YYYYMMDD) is pure and
//                    date-distinct; a boot under an INJECTED FAKE Date derives
//                    that date's weather (no test-only date param in the
//                    product); same fake date + same seed + same script is
//                    byte-identical; a different fake date diverges the world
//                    on the SAME seed
//   7. SPECIES     — two species per tier: childSpeciesFor is pure (pure
//                    pair -> common line, hybrid pair -> rare line); at a
//                    fixed seed both tier-1 species appear deterministically
//                    and harvest values follow the table (fern 5, clover 8);
//                    a same-species pair breeds tier-2 'lotus' and the pure
//                    line ladders to tier-3 'aurora'; a mixed pair breeds
//                    tier-2 'iris' worth 22
//   8. PAUSE       — visibilitychange hidden freezes the loop; visible resumes
//
// Scripted rounds run on a page booted "hidden" (visibility overridden
// BEFORE load, so the game starts paused at frame 0): the rAF loop never
// interleaves with the synchronous __game.step()/tapAt() clock, which is
// what makes the determinism comparison byte-exact.
//
// Driver: playwright-core from a scratch dir (never committed, never
// `playwright install`) + the container's Chromium — same pattern as
// smoke.mjs / run.sh. Run via run-game.sh, or directly:
//   SMOKE_URL=http://127.0.0.1:8902/index.html \
//   NODE_PATH=<scratch>/node_modules node tools/mobile-foundation-smoke/game-smoke.mjs

import { createRequire } from 'node:module';
import { existsSync, readdirSync } from 'node:fs';
import { join } from 'node:path';

const require = createRequire(import.meta.url);
const { chromium } = require('playwright-core');

function findChromium() {
  if (process.env.CHROMIUM_PATH && existsSync(process.env.CHROMIUM_PATH)) {
    return process.env.CHROMIUM_PATH;
  }
  const root = process.env.PLAYWRIGHT_BROWSERS_PATH || '/opt/pw-browsers';
  if (existsSync(root)) {
    for (const d of readdirSync(root)) {
      if (d.startsWith('chromium-')) {
        const p = join(root, d, 'chrome-linux', 'chrome');
        if (existsSync(p)) return p;
      }
    }
    for (const d of readdirSync(root)) {
      if (d.startsWith('chromium_headless_shell-')) {
        const p = join(root, d, 'chrome-linux', 'headless_shell');
        if (existsSync(p)) return p;
      }
    }
  }
  return null;
}

const URL_ = process.env.SMOKE_URL || 'http://127.0.0.1:8902/index.html';
const results = [];
function check(name, ok, detail) {
  results.push({ name, ok, detail });
  console.log(`${ok ? 'PASS' : 'FAIL'}  ${name}  ${detail}`);
}

// Boot a page whose document reports hidden BEFORE any game code runs:
// the game starts paused at frame 0 and only __game.step() advances it.
// Optional fakeDate {y, m, d}: stub Date itself before load, so the REAL
// boot path derives that UTC day's weather — the product ships no
// test-only date parameter.
async function newScriptedPage(browser, url, fakeDate = null) {
  const ctx = await browser.newContext({
    viewport: { width: 390, height: 844 },
    hasTouch: true,
    isMobile: true,
  });
  const page = await ctx.newPage();
  await page.addInitScript(() => {
    Object.defineProperty(document, 'visibilityState', { value: 'hidden', configurable: true });
    Object.defineProperty(document, 'hidden', { value: true, configurable: true });
  });
  if (fakeDate) {
    await page.addInitScript((f) => {
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
    }, fakeDate);
  }
  await page.goto(url, { waitUntil: 'load' });
  await page.waitForFunction(() => window.__gameState && window.__gameState.booted, null, { timeout: 5000 });
  return { ctx, page };
}

const exe = findChromium();
console.log(`game-smoke: chromium = ${exe || '(playwright default)'}`);
console.log(`game-smoke: url      = ${URL_}`);

const browser = await chromium.launch({
  headless: true,
  executablePath: exe || undefined,
  args: ['--no-sandbox', '--disable-gpu'],
});
try {
  // ---- 1. BOOT with an explicit seed -------------------------------------
  const liveCtx = await browser.newContext({
    viewport: { width: 390, height: 844 },
    hasTouch: true,
    isMobile: true,
  });
  const live = await liveCtx.newPage();
  await live.goto(`${URL_}?seed=42`, { waitUntil: 'load' });
  await live.waitForFunction(() => window.__gameState && window.__gameState.booted, null, { timeout: 5000 });
  const b0 = await live.evaluate(() => window.__gameState);
  check('boot-seeded', b0.booted === true && b0.phase === 'playing' && b0.seed === 42,
    `booted=${b0.booted} phase=${b0.phase} seed=${b0.seed} quota=${b0.quota}`);

  // ---- 2. real TouchEvent on the canvas plants a mote ---------------------
  await live.waitForTimeout(300);
  const t0 = await live.evaluate(() => window.__gameState);
  await live.evaluate(() => {
    const c = document.getElementById('game');
    const touch = new Touch({ identifier: 1, target: c, clientX: 60, clientY: 700 });
    c.dispatchEvent(new TouchEvent('touchstart', {
      touches: [touch], changedTouches: [touch], bubbles: true, cancelable: true,
    }));
    c.dispatchEvent(new TouchEvent('touchend', {
      touches: [], changedTouches: [touch], bubbles: true, cancelable: true,
    }));
  });
  const t1 = await live.evaluate(() => window.__gameState);
  check('touchevent-plants',
    t1.taps === t0.taps + 1 && t1.entities === t0.entities + 1 && t1.lastTap !== null,
    `taps ${t0.taps} -> ${t1.taps}, entities ${t0.entities} -> ${t1.entities}, lastTap=${JSON.stringify(t1.lastTap)}`);

  // ---- 3. scripted WIN round ----------------------------------------------
  const winRig = await newScriptedPage(browser, `${URL_}?seed=42`);
  const win = await winRig.page.evaluate(() => {
    const g = window.__game;
    const cv = document.getElementById('game');
    const rect = cv.getBoundingClientRect();
    const kx = rect.width / cv.width, ky = rect.height / cv.height;
    const tap = (x, y) => g.tapAt(rect.left + x * kx, rect.top + y * ky);
    const cw = cv.width, ch = cv.height;
    // two adjacent plants (26 px apart: outside tap-slop, inside pollination)
    tap(cw * 0.15, ch * 0.45);
    tap(cw * 0.15 + 26, ch * 0.45);
    // 18 well-separated singles (>=60 px apart, clear of seeds and the pair)
    for (const fy of [0.6, 0.72, 0.84]) {
      for (const fx of [0.1, 0.27, 0.44, 0.61, 0.78, 0.95]) {
        tap(cw * fx, ch * fy);
      }
    }
    const planted = g.snapshot();
    g.step(310); // everything tier-1 matures; the adjacent pair pollinates
    const matured = g.snapshot();
    const tier2 = matured.motes.filter((m) => m.tier === 2).length;
    // harvest mature tier-1s until the quota falls
    for (const m of matured.motes) {
      if (m.tier !== 1) continue;
      if (g.snapshot().phase !== 'playing') break;
      tap(m.x, m.y);
    }
    return {
      startPaused: window.__gameState.paused,
      planted: { entities: planted.motes.length, taps: planted.taps },
      matured: { entities: matured.motes.length, tier2, frames: matured.frames },
      final: g.snapshot(),
    };
  });
  check('scripted-clock', win.startPaused === true && win.matured.frames === 310,
    `hidden-boot paused=${win.startPaused}, frames after step(310)=${win.matured.frames}`);
  check('pollination-tier2', win.matured.tier2 >= 1 && win.matured.entities === win.planted.entities - 1,
    `tier2 motes=${win.matured.tier2}, entities ${win.planted.entities} -> ${win.matured.entities} (pair merged)`);
  check('win-before-dusk',
    win.final.phase === 'won' && win.final.essence >= 100 && win.final.frames < 5400,
    `phase=${win.final.phase} essence=${win.final.essence}/100 frames=${win.final.frames} (<5400)`);
  await winRig.ctx.close();

  // ---- 4. scripted LOSE round (same seed, nobody gardens) -----------------
  const loseRig = await newScriptedPage(browser, `${URL_}?seed=42`);
  const lose = await loseRig.page.evaluate(() => {
    const g = window.__game;
    g.step(1200);
    const mid = g.snapshot();
    g.step(4200);
    return { mid, final: g.snapshot(), state: window.__gameState };
  });
  check('wisps-hunt', lose.mid.wisps >= 1 && lose.state.motesLost >= 1,
    `wisps alive @step1200=${lose.mid.wisps}, motes eaten by dusk=${lose.state.motesLost}`);
  check('lose-at-dusk', lose.final.phase === 'lost' && lose.final.essence < 100 && lose.final.frames === 5400,
    `phase=${lose.final.phase} essence=${lose.final.essence} frames=${lose.final.frames}`);
  await loseRig.ctx.close();

  // ---- 5. determinism: seed 7 twice, then seed 8 --------------------------
  const script = () => {
    const g = window.__game;
    const cv = document.getElementById('game');
    const rect = cv.getBoundingClientRect();
    const kx = rect.width / cv.width, ky = rect.height / cv.height;
    const at = (x, y) => [rect.left + x * kx, rect.top + y * ky];
    const cw = cv.width, ch = cv.height;
    g.tapAt(...at(cw * 0.3, ch * 0.6));
    g.step(200);
    g.tapAt(...at(cw * 0.62, ch * 0.55));
    g.dragAt(...at(cw * 0.45, ch * 0.57));
    g.step(500);
    g.dragAt(...at(cw * 0.46, ch * 0.58));
    g.step(700);
    g.tapAt(...at(cw * 0.3, ch * 0.6));
    g.step(100);
    return JSON.stringify(g.snapshot());
  };
  const runs = [];
  for (const seed of [7, 7, 8]) {
    const rig = await newScriptedPage(browser, `${URL_}?seed=${seed}`);
    runs.push(await rig.page.evaluate(script));
    await rig.ctx.close();
  }
  check('deterministic-same-seed', runs[0] === runs[1],
    `seed 7 twice -> identical ${runs[0].length}-byte snapshots: ${runs[0].slice(0, 96)}...`);
  check('seed-changes-world', runs[0] !== runs[2],
    `seed 7 vs seed 8 snapshots differ (${runs[0].length}B vs ${runs[2].length}B)`);

  // ---- 6. daily seeded weather ---------------------------------------------
  // Two fixed fake dates with distinct derived weather kinds (pinned below).
  const DAY_A = { y: 2030, m: 6, d: 7, num: 20300607 };  // -> crosswind
  const DAY_B = { y: 2030, m: 6, d: 8, num: 20300608 };  // -> halcyon drift

  // pure derivation: same date twice -> identical params; dates differ
  const wx = await live.evaluate(([a, b]) => {
    const g = window.__game;
    return {
      dayA: JSON.stringify(g.weatherFor(a)),
      dayAAgain: JSON.stringify(g.weatherFor(a)),
      dayB: JSON.stringify(g.weatherFor(b)),
      today: JSON.stringify(g.weatherFor(window.__gameState.weatherDate)),
      state: { weather: window.__gameState.weather, weatherDate: window.__gameState.weatherDate },
    };
  }, [DAY_A.num, DAY_B.num]);
  const todayName = JSON.parse(wx.today).name;
  check('weather-pure-derivation',
    wx.dayA === wx.dayAAgain && wx.dayA !== wx.dayB
      && wx.state.weather === todayName
      && String(wx.state.weatherDate).length === 8,
    `weatherFor(${DAY_A.num}) stable, differs from ${DAY_B.num}; ` +
    `boot weather '${wx.state.weather}' (date ${wx.state.weatherDate}) matches derivation`);

  // fake-date boot: the REAL boot path derives DAY_A's weather
  const wxBootRig = await newScriptedPage(browser, `${URL_}?seed=11`, DAY_A);
  const wxBoot = await wxBootRig.page.evaluate((a) => ({
    weather: window.__gameState.weather,
    weatherDate: window.__gameState.weatherDate,
    derived: window.__game.weatherFor(a).name,
    seed: window.__gameState.seed,
  }), DAY_A.num);
  check('weather-fake-date-boot',
    wxBoot.weatherDate === DAY_A.num && wxBoot.weather === wxBoot.derived && wxBoot.seed === 11,
    `injected ${DAY_A.num} -> weatherDate=${wxBoot.weatherDate} weather='${wxBoot.weather}' (derived '${wxBoot.derived}') seed=${wxBoot.seed}`);
  await wxBootRig.ctx.close();

  // same date + same seed + same script -> byte-identical; a DIFFERENT date
  // diverges the world on the SAME seed (weather is a real sim input)
  const wxScript = () => {
    const g = window.__game;
    g.step(1500);
    return JSON.stringify(g.snapshot());
  };
  const wxRuns = [];
  for (const day of [DAY_A, DAY_A, DAY_B]) {
    const rig = await newScriptedPage(browser, `${URL_}?seed=11`, day);
    wxRuns.push(await rig.page.evaluate(wxScript));
    await rig.ctx.close();
  }
  check('weather-same-date-deterministic', wxRuns[0] === wxRuns[1],
    `${DAY_A.num} twice (seed 11) -> identical ${wxRuns[0].length}-byte snapshots: ${wxRuns[0].slice(0, 96)}...`);
  check('weather-changes-world',
    wxRuns[0] !== wxRuns[2]
      && JSON.parse(wxRuns[0]).seed === JSON.parse(wxRuns[2]).seed
      && JSON.parse(wxRuns[0]).weather !== JSON.parse(wxRuns[2]).weather,
    `same seed 11, ${DAY_A.num} ('${JSON.parse(wxRuns[0]).weather}') vs ${DAY_B.num} ` +
    `('${JSON.parse(wxRuns[2]).weather}') -> snapshots differ`);

  // ---- 7. species -----------------------------------------------------------
  // Fixed seeds pin the SIDE-BAND species stream (mulberry32(seed ^
  // 0x85EBCA6B), one draw per tier-1 spawn: draws 1-3 are the boot seed
  // motes, draws 4+ the player's plants in plant order). Precomputed:
  //   seed 12 -> plants f,f,f,f,c,...   (both kinds within 14 plants)
  //   seed 27 -> plants f,f,f,f          (a pure fern quartet)
  //   seed  9 -> plants f,c              (a hybrid pair)
  // All species pages boot under fake DAY_A so weather drift is pinned too.

  // pure derivation: parents -> child, no game state involved
  const sp0 = await live.evaluate(() => {
    const g = window.__game;
    const table = g.speciesTable();
    return {
      tiers: Object.keys(table).join(','),
      perTier: Object.values(table).map((t) => t.length).join(','),
      pureT1: g.childSpeciesFor(1, 'fern', 'fern').id,
      hybridT1: g.childSpeciesFor(1, 'fern', 'clover').id,
      pureT2: g.childSpeciesFor(2, 'lotus', 'lotus').id,
      hybridT2: g.childSpeciesFor(2, 'lotus', 'iris').id,
    };
  });
  check('species-child-derivation',
    sp0.tiers === '1,2,3' && sp0.perTier === '2,2,2'
      && sp0.pureT1 === 'lotus' && sp0.hybridT1 === 'iris'
      && sp0.pureT2 === 'aurora' && sp0.hybridT2 === 'solaris',
    `table ${sp0.perTier} species over tiers ${sp0.tiers}; ` +
    `fern+fern->${sp0.pureT1} fern+clover->${sp0.hybridT1} ` +
    `lotus+lotus->${sp0.pureT2} lotus+iris->${sp0.hybridT2}`);

  // both tier-1 species appear at seed 12; harvest values follow the table
  const kindsRig = await newScriptedPage(browser, `${URL_}?seed=12`, DAY_A);
  const kinds = await kindsRig.page.evaluate(() => {
    const g = window.__game;
    const cv = document.getElementById('game');
    const rect = cv.getBoundingClientRect();
    const kx = rect.width / cv.width, ky = rect.height / cv.height;
    const tap = (x, y) => g.tapAt(rect.left + x * kx, rect.top + y * ky);
    const cw = cv.width, ch = cv.height;
    // 14 well-separated plants (66 px grid, clear of the boot seed motes)
    let n = 0;
    outer: for (const fy of [0.55, 0.7, 0.85]) {
      for (const fx of [0.1, 0.27, 0.44, 0.61, 0.78]) {
        tap(cw * fx, ch * fy);
        if (++n >= 14) break outer;
      }
    }
    const table = g.speciesTable();
    const valid = g.snapshot().motes.every((m) => table[m.tier].some((s) => s.id === m.sp));
    g.step(310); // all tier-1s mature (grid spacing prevents any merge)
    const mature = g.snapshot();
    const fern = mature.motes.find((m) => m.sp === 'fern');
    const clover = mature.motes.find((m) => m.sp === 'clover');
    const e0 = g.snapshot().essence;
    tap(fern.x, fern.y);
    const e1 = g.snapshot().essence;
    tap(clover.x, clover.y);
    const e2 = g.snapshot().essence;
    return {
      valid,
      kinds: [...new Set(mature.motes.map((m) => m.sp))].sort().join(','),
      count: mature.motes.length,
      fernGain: e1 - e0,
      cloverGain: e2 - e1,
    };
  });
  check('species-both-kinds',
    kinds.valid && kinds.kinds === 'clover,fern' && kinds.count === 17,
    `14 plants + 3 seeds (seed 12) -> ${kinds.count} motes, species {${kinds.kinds}}, all table-valid=${kinds.valid}`);
  check('species-harvest-values', kinds.fernGain === 5 && kinds.cloverGain === 8,
    `harvest fern +${kinds.fernGain} (want 5), clover +${kinds.cloverGain} (want 8)`);
  await kindsRig.ctx.close();

  // pure fern quartet (seed 27): two adjacent pairs -> two tier-2 'lotus';
  // the lotus pair matures and merges -> tier-3 'aurora' (the pure ladder)
  const pureRig = await newScriptedPage(browser, `${URL_}?seed=27`, DAY_A);
  const pure = await pureRig.page.evaluate(() => {
    const g = window.__game;
    const cv = document.getElementById('game');
    const rect = cv.getBoundingClientRect();
    const kx = rect.width / cv.width, ky = rect.height / cv.height;
    const tap = (x, y) => g.tapAt(rect.left + x * kx, rect.top + y * ky);
    const cw = cv.width, ch = cv.height;
    const x = cw * 0.25, y = ch * 0.75;
    tap(x, y); tap(x + 26, y);           // pair A (species draws 4, 5)
    tap(x, y + 34); tap(x + 26, y + 34); // pair B (species draws 6, 7)
    g.step(310); // both pairs mature and merge (one merge per step)
    const mid = g.snapshot();
    // shepherd the two lotus children together with the real drag verb
    // (current() draws no randomness — the herd is fully deterministic),
    // then let them mature and merge
    const herd = () => {
      const t2 = g.snapshot().motes.filter((m) => m.tier === 2);
      if (t2.length === 2) {
        const mx = (t2[0].x + t2[1].x) / 2, my = (t2[0].y + t2[1].y) / 2;
        for (let i = 0; i < 3; i++) g.dragAt(rect.left + mx * kx, rect.top + my * ky);
      }
    };
    herd();
    g.step(150);
    herd();
    g.step(160); // both children mature by ~601 -> pure merge
    const fin = g.snapshot();
    return {
      midT2: mid.motes.filter((m) => m.tier === 2).map((m) => m.sp).join(','),
      midCount: mid.motes.length,
      finT3: fin.motes.filter((m) => m.tier === 3).map((m) => m.sp).join(','),
      finT2: fin.motes.filter((m) => m.tier === 2).length,
      frames: fin.frames,
    };
  });
  check('species-pure-pair', pure.midT2 === 'lotus,lotus' && pure.midCount === 5,
    `seed 27: two fern pairs -> tier-2 [${pure.midT2}], ${pure.midCount} motes (3 seeds + 2 children)`);
  check('species-tier3-ladder', pure.finT3 === 'aurora' && pure.finT2 === 0 && pure.frames === 620,
    `lotus+lotus -> tier-3 [${pure.finT3}] by frame ${pure.frames} (tier-2 left: ${pure.finT2})`);
  await pureRig.ctx.close();

  // hybrid pair (seed 9): fern + clover breed the RARE tier-2 'iris',
  // and harvesting it pays the rare-line value (22)
  const hybRig = await newScriptedPage(browser, `${URL_}?seed=9`, DAY_A);
  const hyb = await hybRig.page.evaluate(() => {
    const g = window.__game;
    const cv = document.getElementById('game');
    const rect = cv.getBoundingClientRect();
    const kx = rect.width / cv.width, ky = rect.height / cv.height;
    const tap = (x, y) => g.tapAt(rect.left + x * kx, rect.top + y * ky);
    const cw = cv.width, ch = cv.height;
    tap(cw * 0.6, ch * 0.7); tap(cw * 0.6 + 26, ch * 0.7); // fern, clover
    const parents = g.snapshot().motes.slice(3).map((m) => m.sp).join('+');
    g.step(310); // pair matures and merges
    const mid = g.snapshot();
    const iris = mid.motes.find((m) => m.tier === 2);
    g.step(310); // the child matures
    const e0 = g.snapshot().essence;
    const grown = g.snapshot().motes.find((m) => m.tier === 2);
    tap(grown.x, grown.y);
    const e1 = g.snapshot().essence;
    return { parents, childSp: iris ? iris.sp : '(none)', irisGain: e1 - e0 };
  });
  check('species-hybrid-pair', hyb.parents === 'fern+clover' && hyb.childSp === 'iris' && hyb.irisGain === 22,
    `seed 9: ${hyb.parents} -> tier-2 '${hyb.childSp}', harvested for +${hyb.irisGain} (want 22)`);
  await hybRig.ctx.close();

  // ---- 8. pause/resume still holds on the live page -----------------------
  await live.evaluate(() => {
    Object.defineProperty(document, 'visibilityState', { value: 'hidden', configurable: true });
    Object.defineProperty(document, 'hidden', { value: true, configurable: true });
    document.dispatchEvent(new Event('visibilitychange'));
  });
  await live.waitForTimeout(120);
  const p0 = await live.evaluate(() => window.__gameState);
  await live.waitForTimeout(400);
  const p1 = await live.evaluate(() => window.__gameState);
  check('pause-on-hidden', p0.paused === true && p1.frames === p0.frames,
    `paused=${p0.paused} frames frozen at ${p0.frames} (recheck ${p1.frames})`);
  await live.evaluate(() => {
    Object.defineProperty(document, 'visibilityState', { value: 'visible', configurable: true });
    Object.defineProperty(document, 'hidden', { value: false, configurable: true });
    document.dispatchEvent(new Event('visibilitychange'));
  });
  await live.waitForTimeout(400);
  const p2 = await live.evaluate(() => window.__gameState);
  check('resume-on-visible', p2.paused === false && p2.frames > p1.frames,
    `paused=${p2.paused} frames ${p1.frames} -> ${p2.frames}`);
  await liveCtx.close();
} finally {
  await browser.close();
}

const failed = results.filter((r) => !r.ok);
console.log(`\ngame-smoke: ${results.length - failed.length}/${results.length} assertions passed`);
if (failed.length) {
  console.log('GAME SMOKE RESULT: FAIL');
  process.exit(1);
}
console.log('GAME SMOKE RESULT: PASS');
