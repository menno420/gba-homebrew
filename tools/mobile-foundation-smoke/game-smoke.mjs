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
//   8. ESSENCE     — essence spending: a won round banks its harvest into
//                    the persistent wallet (fresh storage starts at 0);
//                    buyPalette refuses an unaffordable palette (wallet
//                    unchanged) and a re-buy, and an affordable buy
//                    deducts/unlocks/activates; dusk-screen shop rows
//                    driven by the REAL tap verb select owned palettes
//                    without replanting; a tap off the rows replants with
//                    the wallet intact; a second round's harvest banks on
//                    top (accrual across rounds); wallet/unlocks/active
//                    palette survive a full page reload (guarded
//                    localStorage); and a scripted run with everything
//                    unlocked + a non-default palette active is
//                    byte-identical to the same run on fresh storage
//                    (palettes are pure render, the wallet is meta)
//   9. PAUSE       — visibilitychange hidden freezes the loop; visible resumes
//  10. BIOMES      — essence-purchasable biomes with distinct WIND PATTERNS
//                    (sim parameters read ONCE at round start): the biome
//                    table's default 'meadow' carries identity parameters and
//                    a meadow round reproduces the PRE-CUT baseline snapshots
//                    byte-exactly (fixed point pinned from the pre-biomes
//                    build); every non-default biome is deterministic (same
//                    biome + seed + date twice -> byte-identical two-stage
//                    snapshots) and diverges the world from meadow on the
//                    SAME seed + date; the spend flow denies the
//                    unaffordable/re-buys and deducts on buy; REAL taps on
//                    the dusk-screen biome rows buy/select without replanting
//                    and WITHOUT touching the frozen round's biome; the
//                    selected biome is read at the next replant (the new
//                    round's snapshot carries it); biome unlocks/active
//                    biome survive a full reload; and owning every biome
//                    with meadow active is sim-inert (byte-identical to
//                    fresh storage)
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
// test-only date parameter. Optional initMeta: pre-seed the persistent
// meta wallet/unlocks in localStorage BEFORE the game boots, so the real
// guarded loadMeta() path reads it (no test-only injection point).
async function newScriptedPage(browser, url, fakeDate = null, initMeta = null) {
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
  if (initMeta) {
    await page.addInitScript((m) => {
      // Seed the FIRST boot only: init scripts re-run on reload, and a
      // reload must observe what the game itself persisted, not the seed.
      try {
        if (window.localStorage.getItem('driftgarden.meta') === null) {
          window.localStorage.setItem('driftgarden.meta', JSON.stringify(m));
        }
      } catch (e) { /* storage unavailable — the guarded path defaults */ }
    }, initMeta);
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

  // ---- 8. essence spending --------------------------------------------------
  // One rig carries the whole meta lifecycle: fresh storage -> win a round
  // (the section-3 route, seed 42) -> wallet banks the harvest -> API
  // spend/deny/re-buy -> REAL taps on the dusk-screen shop rows -> replant
  // off the rows -> a second round banks on top -> reload persists it all.
  // Weather is pinned with the fake date so the route is byte-stable.
  const essRig = await newScriptedPage(browser, `${URL_}?seed=42`, DAY_A);
  const ess = await essRig.page.evaluate(() => {
    const g = window.__game;
    const cv = document.getElementById('game');
    const rect = cv.getBoundingClientRect();
    const kx = rect.width / cv.width, ky = rect.height / cv.height;
    const tap = (x, y) => g.tapAt(rect.left + x * kx, rect.top + y * ky);
    const cw = cv.width, ch = cv.height;
    const m0 = g.metaState(); // fresh context: defaults
    // win a round exactly as the WIN assertion does
    tap(cw * 0.15, ch * 0.45);
    tap(cw * 0.15 + 26, ch * 0.45);
    for (const fy of [0.6, 0.72, 0.84]) {
      for (const fx of [0.1, 0.27, 0.44, 0.61, 0.78, 0.95]) tap(cw * fx, ch * fy);
    }
    g.step(310);
    for (const m of g.snapshot().motes) {
      if (m.tier !== 1) continue;
      if (g.snapshot().phase !== 'playing') break;
      tap(m.x, m.y);
    }
    const won = g.snapshot();
    const m1 = g.metaState(); // wallet after round-end banking
    // API spend flow: deny the unaffordable, buy the cheapest affordable,
    // refuse the re-buy
    const table = g.paletteTable();
    const afford = table.find((p) => p.cost > 0 && p.cost <= m1.wallet);
    const tooDear = table.find((p) => p.cost > m1.wallet);
    const denied = tooDear ? g.buyPalette(tooDear.id) : 'no-priced-palette-above-wallet';
    const walletAfterDeny = g.metaState().wallet;
    const bought = g.buyPalette(afford.id);
    const m2 = g.metaState();
    const rebuy = g.buyPalette(afford.id);
    // REAL tap verb on the dusk-screen shop rows (meta UI, no replant)
    const rowCenter = (id) => {
      const r = g.shopRows().find((row) => row.id === id);
      return [rect.left + (r.x + r.w / 2) * kx, rect.top + (r.y + r.h / 2) * ky];
    };
    g.tapAt(...rowCenter('verdant'));           // owned default -> select back
    const afterVerdantTap = { palette: g.metaState().palette, phase: g.snapshot().phase };
    g.tapAt(...rowCenter(afford.id));            // owned bought one -> reselect
    const afterAffordTap = { palette: g.metaState().palette, phase: g.snapshot().phase };
    // a tap OFF the rows replants: new round, wallet intact
    tap(cw * 0.5, ch * 0.08);
    const replanted = g.snapshot();
    const m3 = g.metaState();
    // second round: harvest one boot-seed mote, let dusk fall -> banks on top
    g.step(310);
    const grown = g.snapshot().motes.find((mm) => mm.tier === 1);
    tap(grown.x, grown.y);
    const round2Essence = g.snapshot().essence;
    g.step(5400);
    const dusk2 = g.snapshot();
    const m4 = g.metaState();
    return {
      m0, won: { phase: won.phase, essence: won.essence }, m1,
      afford, tooDear: tooDear ? tooDear.id : null, denied, walletAfterDeny,
      bought, m2, rebuy,
      afterVerdantTap, afterAffordTap,
      replanted: { phase: replanted.phase, essence: replanted.essence, seed: replanted.seed },
      m3, round2Essence, dusk2: { phase: dusk2.phase, essence: dusk2.essence }, m4,
    };
  });
  check('wallet-banks-at-round-end',
    ess.m0.wallet === 0 && ess.m0.palette === 'verdant' && ess.m0.unlocked.join(',') === 'verdant'
      && ess.won.phase === 'won' && ess.m1.wallet === ess.won.essence && ess.m1.wallet >= 100,
    `fresh meta wallet=${ess.m0.wallet}; won with essence=${ess.won.essence} -> wallet=${ess.m1.wallet}`);
  check('spend-deny-buy-rebuy',
    ess.denied === false && ess.walletAfterDeny === ess.m1.wallet
      && ess.bought === true && ess.m2.wallet === ess.m1.wallet - ess.afford.cost
      && ess.m2.unlocked.includes(ess.afford.id) && ess.m2.palette === ess.afford.id
      && ess.rebuy === false,
    `'${ess.tooDear}' denied (wallet ${ess.walletAfterDeny} unchanged); ` +
    `'${ess.afford.id}' bought for ${ess.afford.cost} -> wallet ${ess.m2.wallet}, active; re-buy=false`);
  check('shop-taps-select-not-replant',
    ess.afterVerdantTap.palette === 'verdant' && ess.afterVerdantTap.phase === 'won'
      && ess.afterAffordTap.palette === ess.afford.id && ess.afterAffordTap.phase === 'won',
    `row taps: verdant -> active '${ess.afterVerdantTap.palette}' (phase ${ess.afterVerdantTap.phase}), ` +
    `${ess.afford.id} -> active '${ess.afterAffordTap.palette}' (phase ${ess.afterAffordTap.phase})`);
  check('replant-keeps-wallet-and-accrues',
    ess.replanted.phase === 'playing' && ess.replanted.essence === 0 && ess.replanted.seed === 43
      && ess.m3.wallet === ess.m2.wallet
      && ess.dusk2.phase === 'lost' && ess.round2Essence > 0
      && ess.m4.wallet === ess.m3.wallet + ess.dusk2.essence,
    `off-row tap replanted (seed ${ess.replanted.seed}, essence 0, wallet ${ess.m3.wallet}); ` +
    `round 2 banked +${ess.dusk2.essence} at dusk -> wallet ${ess.m4.wallet}`);

  // full page reload in the SAME context: guarded localStorage carries the
  // wallet, the unlock, and the active palette through the real boot path
  await essRig.page.reload({ waitUntil: 'load' });
  await essRig.page.waitForFunction(() => window.__gameState && window.__gameState.booted, null, { timeout: 5000 });
  const persisted = await essRig.page.evaluate(() => ({
    meta: window.__game.metaState(),
    state: {
      wallet: window.__gameState.wallet,
      palette: window.__gameState.palette,
      unlocked: window.__gameState.unlocked,
    },
  }));
  check('meta-persists-reload',
    persisted.meta.wallet === ess.m4.wallet
      && persisted.meta.palette === ess.afford.id
      && persisted.meta.unlocked.join(',') === `verdant,${ess.afford.id}`
      && persisted.state.wallet === ess.m4.wallet && persisted.state.palette === ess.afford.id
      && persisted.state.unlocked === `verdant,${ess.afford.id}`,
    `reload -> wallet ${persisted.meta.wallet}, unlocked [${persisted.meta.unlocked.join(',')}], ` +
    `active '${persisted.meta.palette}' (state mirror agrees)`);
  await essRig.ctx.close();

  // sim identity: the section-5 script on seed 7, fresh storage vs a
  // pre-seeded wallet with EVERY palette unlocked and a non-default one
  // active -> byte-identical snapshots (unlocks are pure render/meta)
  const freshRig = await newScriptedPage(browser, `${URL_}?seed=7`, DAY_A);
  const freshRun = await freshRig.page.evaluate(script);
  const freshPal = await freshRig.page.evaluate(() => window.__gameState.palette);
  await freshRig.ctx.close();
  const richMeta = {
    wallet: 500,
    unlocked: ['verdant', 'duskbloom', 'moonlit', 'emberdawn'],
    palette: 'moonlit',
    // every biome OWNED but the default ACTIVE: biome ownership is meta —
    // only the active biome (read at round start) is a sim input
    biomes: ['meadow', 'galeridge', 'whorl', 'tideflat'],
    biome: 'meadow',
  };
  const richRig = await newScriptedPage(browser, `${URL_}?seed=7`, DAY_A, richMeta);
  const richRun = await richRig.page.evaluate(script);
  const richState = await richRig.page.evaluate(() => ({
    palette: window.__gameState.palette,
    wallet: window.__gameState.wallet,
    unlocked: window.__gameState.unlocked,
  }));
  await richRig.ctx.close();
  check('unlocks-inert-to-sim',
    richState.palette === 'moonlit' && richState.wallet === 500
      && richState.unlocked.split(',').length === 4 && freshPal === 'verdant'
      && freshRun === richRun,
    `seed 7 script: fresh storage ('${freshPal}') vs wallet 500 + all palettes + '${richState.palette}' ` +
    `active -> identical ${freshRun.length}-byte snapshots`);

  // ---- 10. biomes (wind patterns per biome) ---------------------------------
  // The round's biome is a sim input read ONCE at round start (like the
  // day's weather at boot). Two-stage probe: snapshot at step 500 (the
  // three boot motes' wind-drifted positions) and step 1500 (wisps have
  // hunted). PRE-CUT fixed point, captured from the pre-biomes build
  // (main @ 65c3659) at seed 11 under fake DAY_A: the default 'meadow'
  // biome must reproduce these bytes exactly (modulo the added biome key,
  // stripped before comparison).
  const PRECUT_500 = '{"frames":500,"seed":11,"weather":"crosswind","weatherDate":20300607,'
    + '"phase":"playing","essence":0,"taps":0,"wisps":0,"motes":['
    + '{"tier":1,"sp":"clover","x":109.21,"y":280.15},'
    + '{"tier":1,"sp":"fern","x":211.05,"y":283.08},'
    + '{"tier":1,"sp":"fern","x":302.89,"y":282.29}]}';
  const PRECUT_1500 = '{"frames":1500,"seed":11,"weather":"crosswind","weatherDate":20300607,'
    + '"phase":"playing","essence":0,"taps":0,"wisps":2,"motes":[]}';
  const bioScript = () => {
    const g = window.__game;
    g.step(500);
    const a = JSON.stringify(g.snapshot());
    g.step(1000);
    return a + '\n' + JSON.stringify(g.snapshot());
  };
  const stripBiome = (two) => two.split('\n').map((s) => {
    const o = JSON.parse(s);
    delete o.biome;
    return JSON.stringify(o);
  }).join('\n');

  // table + defaults + row geometry (live page: fresh meta, real date)
  const bio0 = await live.evaluate(() => {
    const g = window.__game;
    const t = g.biomeTable();
    const meadow = t[0];
    const rows = g.biomeRows();
    const prows = g.shopRows();
    const plast = prows[prows.length - 1];
    return {
      n: t.length,
      ids: t.map((b) => b.id).join(','),
      meadowIdentity: meadow.id === 'meadow' && meadow.cost === 0
        && meadow.windMul === 1 && meadow.biasMul === 1 && meadow.gustEvery === 600
        && meadow.swirlRate === 0 && meadow.swayPeriod === 0 && meadow.tint === 0,
      othersPriced: t.slice(1).every((b) => b.cost > 0),
      meta: g.metaState(),
      snapBiome: g.snapshot().biome,
      roundBiome: window.__gameState.roundBiome,
      rowsBelowPalettes: rows.length === t.length && rows[0].y > plast.y + plast.h,
    };
  });
  check('biome-table-and-default',
    bio0.n === 4 && bio0.ids === 'meadow,galeridge,whorl,tideflat'
      && bio0.meadowIdentity && bio0.othersPriced
      && bio0.meta.biomes.join(',') === 'meadow' && bio0.meta.biome === 'meadow'
      && bio0.snapBiome === 'meadow' && bio0.roundBiome === 'meadow'
      && bio0.rowsBelowPalettes,
    `${bio0.n} biomes [${bio0.ids}]; meadow = identity params, cost 0; ` +
    `fresh meta owns/activates 'meadow'; snapshot biome '${bio0.snapBiome}'; ` +
    `biome rows sit below palette rows=${bio0.rowsBelowPalettes}`);

  // default biome reproduces the PRE-CUT baseline byte-exactly
  const idRig = await newScriptedPage(browser, `${URL_}?seed=11`, DAY_A);
  const meadowRun = await idRig.page.evaluate(bioScript);
  await idRig.ctx.close();
  const meadowBiomes = meadowRun.split('\n').map((s) => JSON.parse(s).biome).join(',');
  check('biome-default-precut-identity',
    stripBiome(meadowRun) === `${PRECUT_500}\n${PRECUT_1500}` && meadowBiomes === 'meadow,meadow',
    `meadow @ seed 11/${DAY_A.num}: steps 500+1500 match the pre-cut build's ` +
    `pinned snapshots byte-exactly (biome key '${meadowBiomes.split(',')[0]}' stripped)`);

  // each purchasable biome: same biome + seed + date twice -> identical;
  // and every biome diverges the world from meadow on the SAME seed + date
  const biomeRuns = { meadow: meadowRun };
  for (const id of ['galeridge', 'whorl', 'tideflat']) {
    const im = { wallet: 0, unlocked: ['verdant'], palette: 'verdant', biomes: ['meadow', id], biome: id };
    const pair = [];
    for (let i = 0; i < 2; i++) {
      const rig = await newScriptedPage(browser, `${URL_}?seed=11`, DAY_A, im);
      pair.push(await rig.page.evaluate(bioScript));
      await rig.ctx.close();
    }
    biomeRuns[id] = pair[0];
    biomeRuns[`${id}-repeat-ok`] = pair[0] === pair[1]
      && pair[0].split('\n').every((s) => JSON.parse(s).biome === id);
  }
  check('biome-deterministic-per-biome',
    biomeRuns['galeridge-repeat-ok'] && biomeRuns['whorl-repeat-ok'] && biomeRuns['tideflat-repeat-ok'],
    `seed 11/${DAY_A.num} twice per biome -> byte-identical two-stage snapshots ` +
    `(galeridge ${biomeRuns.galeridge.length}B, whorl ${biomeRuns.whorl.length}B, ` +
    `tideflat ${biomeRuns.tideflat.length}B), each tagged with its biome`);
  const worlds = ['meadow', 'galeridge', 'whorl', 'tideflat'].map((id) => stripBiome(biomeRuns[id]));
  const allDistinct = new Set(worlds).size === 4;
  const sameInputs = worlds.every((w) => {
    const o = JSON.parse(w.split('\n')[0]);
    return o.seed === 11 && o.weatherDate === DAY_A.num;
  });
  check('biome-changes-world', allDistinct && sameInputs,
    `same seed 11 + date ${DAY_A.num}: meadow/galeridge/whorl/tideflat -> ` +
    `4 pairwise-distinct worlds (biome key stripped before comparing)`);

  // spend flow + REAL dusk-screen taps + round-start read + persistence
  const bioMeta = { wallet: 300, unlocked: ['verdant'], palette: 'verdant', biomes: ['meadow'], biome: 'meadow' };
  const bioRig = await newScriptedPage(browser, `${URL_}?seed=5`, DAY_A, bioMeta);
  const bio = await bioRig.page.evaluate(() => {
    const g = window.__game;
    const cv = document.getElementById('game');
    const rect = cv.getBoundingClientRect();
    const kx = rect.width / cv.width, ky = rect.height / cv.height;
    const tap = (x, y) => g.tapAt(rect.left + x * kx, rect.top + y * ky);
    const cw = cv.width, ch = cv.height;
    const m0 = g.metaState();
    g.step(5400); // nobody gardens -> dusk falls, the shop opens
    const dusk = g.snapshot();
    // API flow: buy the dearest, get denied on the next, refuse a re-buy
    // and an unowned select
    const buyBig = g.buyBiome('tideflat');       // 300 - 220 -> 80
    const denied = g.buyBiome('whorl');          // 130 > 80
    const walletAfterDeny = g.metaState().wallet;
    const rebuy = g.buyBiome('tideflat');
    const selUnowned = g.selectBiome('whorl');
    const m1 = g.metaState();
    // REAL tap verb on the biome rows: buy galeridge (60 <= 80), then
    // select tideflat back — the frozen round's biome must not move
    const rowCenter = (id) => {
      const r = g.biomeRows().find((row) => row.id === id);
      return [rect.left + (r.x + r.w / 2) * kx, rect.top + (r.y + r.h / 2) * ky];
    };
    g.tapAt(...rowCenter('galeridge'));
    const afterGaleTap = {
      biome: g.metaState().biome, wallet: g.metaState().wallet,
      owned: g.metaState().biomes.join(','), phase: g.snapshot().phase,
      frozenBiome: g.snapshot().biome,
    };
    g.tapAt(...rowCenter('tideflat'));
    const afterTideTap = { biome: g.metaState().biome, phase: g.snapshot().phase };
    // off the rows: replant — the NEW round reads the selected biome
    tap(cw * 0.5, ch * 0.05);
    const replanted = g.snapshot();
    return {
      m0, dusk: { phase: dusk.phase, biome: dusk.biome },
      buyBig, denied, walletAfterDeny, rebuy, selUnowned, m1,
      afterGaleTap, afterTideTap,
      replanted: { phase: replanted.phase, seed: replanted.seed, biome: replanted.biome },
      m2: g.metaState(),
    };
  });
  check('biome-spend-deny-buy',
    bio.m0.biomes.join(',') === 'meadow' && bio.dusk.phase === 'lost' && bio.dusk.biome === 'meadow'
      && bio.buyBig === true && bio.m1.wallet === 80 && bio.denied === false
      && bio.walletAfterDeny === 80 && bio.rebuy === false && bio.selUnowned === false
      && bio.m1.biome === 'tideflat' && bio.m1.biomes.join(',') === 'meadow,tideflat',
    `wallet 300: tideflat bought for 220 -> ${bio.m1.wallet}, active; whorl (130) denied ` +
    `(wallet ${bio.walletAfterDeny} unchanged); re-buy=${bio.rebuy} unowned-select=${bio.selUnowned}`);
  check('biome-shop-taps-frozen-round',
    bio.afterGaleTap.biome === 'galeridge' && bio.afterGaleTap.wallet === 20
      && bio.afterGaleTap.owned === 'meadow,tideflat,galeridge'
      && bio.afterGaleTap.phase === 'lost' && bio.afterGaleTap.frozenBiome === 'meadow'
      && bio.afterTideTap.biome === 'tideflat' && bio.afterTideTap.phase === 'lost',
    `row taps: galeridge bought for 60 -> wallet ${bio.afterGaleTap.wallet}, active; ` +
    `tideflat re-selected; frozen round stays phase '${bio.afterTideTap.phase}' ` +
    `biome '${bio.afterGaleTap.frozenBiome}' throughout`);
  check('biome-read-at-round-start',
    bio.replanted.phase === 'playing' && bio.replanted.seed === 6
      && bio.replanted.biome === 'tideflat' && bio.m2.wallet === 20,
    `off-row tap replanted (seed ${bio.replanted.seed}) -> the new round runs ` +
    `biome '${bio.replanted.biome}' with wallet ${bio.m2.wallet} intact`);

  // full reload in the SAME context: biome unlocks + active biome persist,
  // and the booted round reads the persisted choice
  await bioRig.page.reload({ waitUntil: 'load' });
  await bioRig.page.waitForFunction(() => window.__gameState && window.__gameState.booted, null, { timeout: 5000 });
  const bioPersist = await bioRig.page.evaluate(() => ({
    meta: window.__game.metaState(),
    roundBiome: window.__gameState.roundBiome,
    snapBiome: window.__game.snapshot().biome,
    stateBiomes: window.__gameState.biomes,
  }));
  check('biome-persists-reload',
    bioPersist.meta.biomes.join(',') === 'meadow,tideflat,galeridge'
      && bioPersist.meta.biome === 'tideflat' && bioPersist.meta.wallet === 20
      && bioPersist.roundBiome === 'tideflat' && bioPersist.snapBiome === 'tideflat'
      && bioPersist.stateBiomes === 'meadow,tideflat,galeridge',
    `reload -> owned [${bioPersist.meta.biomes.join(',')}], active '${bioPersist.meta.biome}', ` +
    `wallet ${bioPersist.meta.wallet}; the booted round runs '${bioPersist.roundBiome}'`);
  await bioRig.ctx.close();

  // ---- 9. pause/resume still holds on the live page -----------------------
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
