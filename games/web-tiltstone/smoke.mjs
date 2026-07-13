#!/usr/bin/env node
/* Tiltstone engine smoke — pure Node, no browser, no dependencies.
 *
 * Loads games/web-tiltstone/engine.js (the same file the page ships) and
 * plays seeded games start-to-finish with ASSERTED state transitions:
 *
 *   1. seed 42 / level 0: pinned initial grid, quota, budget, salt
 *   2. mechanics unit truths: rotation orientation, gravity settle around a
 *      wall, a hand-built 3-in-a-column merge that collects + cascades
 *   3. scripted rotation on seed 42: gravity settles everything to rest,
 *      a merge/collect event fires with the pinned gem count, the rotation
 *      budget decrements
 *   4. WIN: replaying the generator's own solver line reaches 'won'
 *   5. LOSS: a constructed no-collect budget burn reaches 'lost'
 *   6. determinism: same seed twice -> byte-identical state trace
 *   7. purity: rotate() never mutates its input state
 *   8. generation sweep: 12 consecutive seeds all produce solvable levels
 *      whose solver line actually wins
 *   9. daily seed: pure date -> integer mapping
 *  10. par + grade (slice 2): par == solution length, BFS-verified MINIMAL
 *      (no shorter winning line exists within budget), grade truth table,
 *      and par minimality re-verified across the 12-seed sweep
 *
 * Exits nonzero on any failed assertion; prints one PASS/FAIL line each.
 */
import { createRequire } from "node:module";
import { fileURLToPath } from "node:url";
import path from "node:path";

const here = path.dirname(fileURLToPath(import.meta.url));
const require = createRequire(import.meta.url);
const E = require(path.join(here, "engine.js"));

let failures = 0;
function check(name, ok, detail = "") {
  console.log(`${ok ? "PASS" : "FAIL"}: ${name}${detail ? ` — ${detail}` : ""}`);
  if (!ok) failures++;
}

// ---------------------------------------------------------------- 1. gen pin
const SEED = 42;
const g0 = E.newGame(SEED, 0);
const PINNED_GRID_42 = [
  "B......#",
  "A.......",
  "#.......",
  "#..B....",
  ".o.#...C",
  "#AA.C..o",
  ".Co.B.AA",
  "o#B.C.BC"
].join("\n");
check("seed 42 level 0: pinned initial grid", E.gridString(g0.grid) === PINNED_GRID_42,
  `grid ${E.gridString(g0.grid) === PINNED_GRID_42 ? "matches 8x8 pin" : "DIVERGED:\n" + E.gridString(g0.grid)}`);
check("seed 42: pinned level params", g0.quota === 7 && g0.budget === 10 && g0.level.salt === 0 && g0.level.best === 11,
  `quota=${g0.quota} budget=${g0.budget} salt=${g0.level.salt} best=${g0.level.best}`);
check("initial state is at rest and merge-free",
  E.gridString(E.settle(g0.grid)) === E.gridString(g0.grid) && E.findGroups(g0.grid).length === 0,
  "settle(grid)==grid, 0 groups");

// ------------------------------------------------------- 2. mechanics truths
{
  // rotation orientation: a lone marker at top-left goes to top-right on CW
  const m = E.emptyGrid(8); m[0][0] = E.STONE;
  const cw = E.rotateGrid(m, "cw"), ccw = E.rotateGrid(m, "ccw");
  check("rotation orientation (CW moves [0][0] -> [0][7], CCW -> [7][0])",
    cw[0][7] === E.STONE && ccw[7][0] === E.STONE);

  // gravity: a gem above a wall rests ON the wall; beside it falls to floor
  const w = E.emptyGrid(8); w[4][2] = E.WALL; w[0][2] = E.GEM0; w[0][3] = E.GEM0;
  const ws = E.settle(w);
  check("gravity settle: wall support vs open column",
    ws[3][2] === E.GEM0 && ws[7][3] === E.GEM0 && ws[0][2] === E.EMPTY,
    `on-wall row=3, open-column row=7`);

  // merge + cascade: column of 3 reds collects; a blue pair + one blue that
  // falls into place after the reds vanish chains to a second collect
  const c = E.emptyGrid(8);
  c[7][0] = E.GEM0; c[6][0] = E.GEM0; c[5][0] = E.GEM0;      // 3 reds stacked
  c[4][0] = E.GEM0 + 1;                                       // blue on top of reds
  c[7][1] = E.GEM0 + 1; c[7][2] = E.GEM0 + 1;                 // 2 blues on floor
  const res = E.resolve(c);
  const chains = res.events.map(e => `${e.chain}:c${e.color}x${e.size}`).join(",");
  check("merge collects 3+ and CASCADES a chain",
    res.collected === 6 && res.events.length === 2 && res.events[1].chain === 2,
    `collected=${res.collected} events=[${chains}]`);
}

// ------------------------------------- 3. scripted rotation on the real level
{
  const s1 = E.rotate(g0, "cw");
  check("rotation decrements budget (used 0 -> 1)", g0.used === 0 && s1.used === 1,
    `used=${s1.used}/${s1.budget}`);
  check("grid at rest after rotation (gravity settled)",
    E.gridString(E.settle(s1.grid)) === E.gridString(s1.grid));
  // find the first scripted prefix of the solution that fires a collect event
  let st = g0, evAt = -1, evDetail = "";
  for (let i = 0; i < g0.level.solution.length; i++) {
    st = E.rotate(st, g0.level.solution[i] === "R" ? "cw" : "ccw");
    if (st.events.length && evAt === -1) {
      evAt = i + 1;
      evDetail = st.events.map(e => `c${e.color}x${e.size}@chain${e.chain}`).join(",");
    }
  }
  check("a merge/collect event fires on the solver line", evAt !== -1,
    `first collect after rotation #${evAt}: ${evDetail}`);
  check("collected count tracks events", st.collected >= g0.quota,
    `collected=${st.collected} quota=${g0.quota}`);
}

// -------------------------------------------------------------------- 4. WIN
{
  const w = E.replay(E.newGame(SEED, 0), g0.level.solution);
  check("WIN: replaying the generator's solver line reaches 'won'",
    w.status === "won" && w.collected >= w.quota && w.used === g0.level.solution.length,
    `solution="${g0.level.solution}" -> status=${w.status} collected=${w.collected}/${w.quota} used=${w.used}/${w.budget}`);
  const extra = E.rotate(w, "cw");
  check("terminal state is frozen (rotate after win is a no-op)", extra === w);
}

// ------------------------------------------------------------------- 5. LOSS
{
  // Constructed loss: alternate L/R (each undone before anything can line up
  // on this layout) until the budget is gone without reaching quota.
  let st = E.newGame(SEED, 0);
  const burn = [];
  while (st.status === "playing") {
    // pick whichever direction collects LESS this turn (greedy anti-play)
    const a = E.rotate(st, "cw"), b = E.rotate(st, "ccw");
    const pick = (a.collected <= b.collected) ? ["R", a] : ["L", b];
    burn.push(pick[0]); st = pick[1];
  }
  check("LOSS: anti-play burns the budget to 'lost'",
    st.status === "lost" && st.used === st.budget && st.collected < st.quota,
    `line="${burn.join("")}" -> status=${st.status} collected=${st.collected}/${st.quota} used=${st.used}/${st.budget}`);
}

// ----------------------------------------------------------- 6. determinism
{
  const trace = (seed, line) => {
    let st = E.newGame(seed, 0);
    const out = [E.gridString(st.grid)];
    for (const ch of line) { st = E.rotate(st, ch === "R" ? "cw" : "ccw"); out.push(E.gridString(st.grid) + "|" + st.collected + "|" + st.status); }
    return out.join("\n---\n");
  };
  const line = "RLRRLLRRLR".slice(0, g0.budget);
  check("determinism: same seed + same rotations -> identical full state trace",
    trace(SEED, line) === trace(SEED, line));
  check("different seed -> different level", E.gridString(E.newGame(43, 0).grid) !== PINNED_GRID_42);
}

// ----------------------------------------------------------------- 7. purity
{
  const before = E.gridString(g0.grid) + `|${g0.used}|${g0.collected}|${g0.status}`;
  E.rotate(g0, "ccw");
  const after = E.gridString(g0.grid) + `|${g0.used}|${g0.collected}|${g0.status}`;
  check("purity: rotate() never mutates its input state", before === after);
}

// ------------------------------------------------------- 8. generation sweep
{
  let ok = 0, tried = 0, worstSalt = 0;
  for (let s = 100; s < 112; s++) {
    tried++;
    const lvl = E.generateLevel(s, 0);
    worstSalt = Math.max(worstSalt, lvl.salt);
    const end = E.replay(E.newGame(s, 0), lvl.solution);
    if (end.status === "won") ok++;
  }
  check("generation sweep: 12 seeds, every solver line wins its own level",
    ok === tried, `${ok}/${tried} won, worst salt=${worstSalt}`);
  const l1 = E.generateLevel(SEED, 1);
  check("level chain: (seed 42, level 1) generates solvable too",
    l1.solution !== null && E.replay(E.newGame(SEED, 1), l1.solution).status === "won",
    `quota=${l1.quota} best=${l1.best} solution="${l1.solution}"`);
}

// ------------------------------------------------------------- 9. daily seed
check("daily seed is a pure date mapping", E.dailySeed("2026-07-13") === 20260713,
  `2026-07-13 -> ${E.dailySeed("2026-07-13")}`);

// -------------------------------------------------- 10. par + grade (slice 2)
{
  // helper: independently derive the true shortest winning depth via search()
  const minWinDepth = (level) => {
    const found = E.search(level.grid, level.budget);
    let min = Infinity;
    for (const rec of found.records)
      if (rec.collected >= level.quota && rec.depth < min) min = rec.depth;
    return min;
  };

  const p = E.par(g0.level);
  check("par == stored solution length", p === g0.level.solution.length,
    `par=${p} solution="${g0.level.solution}"`);
  check("par is MINIMAL: BFS finds no shorter winning line (seed 42)",
    p === minWinDepth(g0.level), `search min win depth=${minWinDepth(g0.level)}`);

  // grade truth table around par (par floors the win, diff clamps at 0)
  const table = [0, 1, 2, 3, 5].map(d => E.grade(p + d, p).label).join(",");
  check("grade truth table: 0->PERFECT 1->GREAT 2->GOOD 3+->CLEARED",
    table === "PERFECT,GREAT,GOOD,CLEARED,CLEARED" && E.grade(p - 1, p).diff === 0,
    `[${table}], underflow clamps to diff 0`);

  // a PERFECT is reachable by construction: the solver line wins in par turns
  const perfect = E.replay(E.newGame(SEED, 0), g0.level.solution);
  check("replaying the solver line is graded PERFECT",
    perfect.status === "won" && E.grade(perfect.used, p).label === "PERFECT",
    `used=${perfect.used} par=${p}`);

  // par minimality holds across the same 12-seed sweep as section 8
  let minimalOk = 0;
  for (let s = 100; s < 112; s++) {
    const lvl = E.generateLevel(s, 0);
    if (E.par(lvl) === minWinDepth(lvl)) minimalOk++;
  }
  check("12-seed sweep: every stored par is the true BFS minimum",
    minimalOk === 12, `${minimalOk}/12 minimal`);
}

// ------------------------------------------------------------------- verdict
if (failures) {
  console.error(`SMOKE FAIL: ${failures} assertion(s) red`);
  process.exit(1);
}
console.log(`SMOKE PASS: all assertions green (engine v${E.VERSION}, seed=${SEED}, solution="${g0.level.solution}")`);
