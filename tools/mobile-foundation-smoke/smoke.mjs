#!/usr/bin/env node
// Headless smoke proof for games/mobile-foundation (Drift Garden skeleton).
//
// Asserts, against the real served page in headless Chromium:
//   1. BOOT      — window.__gameState.booted is true after load
//   2. LOOP      — the fixed-step frame counter advances over wall time
//   3. TAP       — a synthetic tap (touch context) increments taps + entities
//   4. PAUSE     — hiding the page pauses the loop (frame counter freezes,
//                  paused=true observable); un-hiding resumes it
//
// Pause/resume honesty: headless Chromium cannot literally background a tab,
// so visibility is driven the way Playwright does it for every browser —
// the page's document.visibilityState is overridden and a real
// `visibilitychange` event is dispatched. The game's OWN handler runs; only
// the browser-level trigger is simulated.
//
// Driver: playwright-core (NOT committed — install into any scratch dir,
// `npm i playwright-core`, and run with NODE_PATH pointing at it), against
// the container's pre-provisioned Chromium (PLAYWRIGHT_BROWSERS_PATH or
// CHROMIUM_PATH). Never run `playwright install`.
//
// Usage:
//   cd <repo>/games/mobile-foundation && python3 -m http.server 8901 &
//   NODE_PATH=<scratch>/node_modules node tools/mobile-foundation-smoke/smoke.mjs
// Optional env: SMOKE_URL (default http://127.0.0.1:8901/index.html),
//               CHROMIUM_PATH (explicit browser binary).

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
  return null; // let playwright-core try its own resolution
}

const URL_ = process.env.SMOKE_URL || 'http://127.0.0.1:8901/index.html';
const results = [];
function check(name, ok, detail) {
  results.push({ name, ok, detail });
  console.log(`${ok ? 'PASS' : 'FAIL'}  ${name}  ${detail}`);
}

const exe = findChromium();
console.log(`smoke: chromium = ${exe || '(playwright default)'}`);
console.log(`smoke: url      = ${URL_}`);

const browser = await chromium.launch({
  headless: true,
  executablePath: exe || undefined,
  args: ['--no-sandbox', '--disable-gpu'],
});
try {
  const ctx = await browser.newContext({
    viewport: { width: 390, height: 844 }, // phone-shaped
    hasTouch: true,
    isMobile: true,
  });
  const page = await ctx.newPage();
  await page.goto(URL_, { waitUntil: 'load' });

  // 1. BOOT
  await page.waitForFunction(() => window.__gameState && window.__gameState.booted, null, { timeout: 5000 });
  const s0 = await page.evaluate(() => window.__gameState);
  check('boot', s0.booted === true && s0.entities >= 3,
    `booted=${s0.booted} entities=${s0.entities}`);

  // 2. LOOP ticks
  await page.waitForTimeout(600);
  const s1 = await page.evaluate(() => window.__gameState);
  check('loop-ticks', s1.frames > s0.frames && s1.renders > s0.renders,
    `frames ${s0.frames} -> ${s1.frames}, renders ${s0.renders} -> ${s1.renders}`);

  // 3. TAP changes state (real synthetic touch via the touch-enabled context)
  await page.touchscreen.tap(195, 400);
  await page.waitForTimeout(150);
  const s2 = await page.evaluate(() => window.__gameState);
  check('tap-changes-state',
    s2.taps === s1.taps + 1 && s2.entities === s1.entities + 1 && s2.lastTap !== null,
    `taps ${s1.taps} -> ${s2.taps}, entities ${s1.entities} -> ${s2.entities}, lastTap=${JSON.stringify(s2.lastTap)}`);

  // DOM mirror agrees
  const hud = await page.evaluate(() => document.getElementById('hud').dataset.entities);
  check('dom-mirror', Number(hud) === s2.entities, `hud entities=${hud}`);

  // 4. PAUSE via visibilitychange (simulated hidden state, real handler)
  await page.evaluate(() => {
    Object.defineProperty(document, 'visibilityState', { value: 'hidden', configurable: true });
    Object.defineProperty(document, 'hidden', { value: true, configurable: true });
    document.dispatchEvent(new Event('visibilitychange'));
  });
  await page.waitForTimeout(120);
  const p0 = await page.evaluate(() => window.__gameState);
  await page.waitForTimeout(400);
  const p1 = await page.evaluate(() => window.__gameState);
  check('pause-on-hidden', p0.paused === true && p1.frames === p0.frames,
    `paused=${p0.paused} frames frozen at ${p0.frames} (recheck ${p1.frames})`);

  // resume
  await page.evaluate(() => {
    Object.defineProperty(document, 'visibilityState', { value: 'visible', configurable: true });
    Object.defineProperty(document, 'hidden', { value: false, configurable: true });
    document.dispatchEvent(new Event('visibilitychange'));
  });
  await page.waitForTimeout(400);
  const p2 = await page.evaluate(() => window.__gameState);
  check('resume-on-visible', p2.paused === false && p2.frames > p1.frames,
    `paused=${p2.paused} frames ${p1.frames} -> ${p2.frames}`);
} finally {
  await browser.close();
}

const failed = results.filter((r) => !r.ok);
console.log(`\nsmoke: ${results.length - failed.length}/${results.length} assertions passed`);
if (failed.length) {
  console.log('SMOKE RESULT: FAIL');
  process.exit(1);
}
console.log('SMOKE RESULT: PASS');
