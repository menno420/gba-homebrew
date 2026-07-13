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
 *  11. trace + juice (slice 3): settleMoves/resolveTrace agree with
 *      settle/resolve byte-for-byte (incl. the 12-seed sweep), the phase
 *      script is well-formed, and juice.js's pure timeline + synth cue
 *      table are deterministic (chain-rising collect pitch, capped)
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

// ----------------------------------------- 11. trace + juice pure (slice 3)
{
  const J = require(path.join(here, "juice.js"));

  // settleMoves: same grid as settle, and the move list reconstructs it.
  // (Fixture from section 2: gem over a wall + gem in an open column.)
  const w = E.emptyGrid(8); w[4][2] = E.WALL; w[0][2] = E.GEM0; w[0][3] = E.GEM0;
  const sm = E.settleMoves(w);
  const rebuilt = E.cloneGrid(w);
  for (const mv of sm.moves) rebuilt[mv.from[0]][mv.from[1]] = E.EMPTY;
  for (const mv of sm.moves) rebuilt[mv.to[0]][mv.to[1]] = mv.v;
  check("settleMoves: grid byte-identical to settle, moves reconstruct it",
    E.gridString(sm.grid) === E.gridString(E.settle(w)) &&
    E.gridString(rebuilt) === E.gridString(sm.grid) &&
    sm.moves.length === 2 && sm.moves.some(m => m.to[0] === 3 && m.to[1] === 2) &&
    sm.moves.some(m => m.to[0] === 7 && m.to[1] === 3),
    `moves=${sm.moves.map(m => `[${m.from}]->[${m.to}]`).join(" ")}`);

  // resolveTrace ≡ resolve on every rotation of the seed-42 solver line,
  // and the phase script is well-formed.
  let st42 = E.newGame(SEED, 0), traceOk = true, phasesOk = true, detail = "";
  for (const ch of g0.level.solution) {
    const rot = E.rotateGrid(st42.grid, ch === "R" ? "cw" : "ccw");
    const plain = E.resolve(rot), trace = E.resolveTrace(rot);
    if (E.gridString(trace.grid) !== E.gridString(plain.grid) ||
        trace.collected !== plain.collected ||
        JSON.stringify(trace.events) !== JSON.stringify(plain.events)) traceOk = false;
    if (trace.phases[0].type !== "fall" ||
        E.gridString(trace.phases[trace.phases.length - 1].grid) !== E.gridString(trace.grid) ||
        trace.phases.some(p => p.type === "collect" && !(p.chain >= 1 && p.cells.length === p.size))) phasesOk = false;
    detail += `${ch}:${trace.phases.length}ph/${trace.collected}g `;
    st42 = E.rotate(st42, ch === "R" ? "cw" : "ccw");
  }
  check("resolveTrace ≡ resolve (grid+collected+events) on the seed-42 solver line", traceOk, detail.trim());
  check("trace phases well-formed (fall-first, last grid == final, collect fields sane)", phasesOk);

  // consistency across the 12-seed sweep: the trace's final grid is exactly
  // what rotate() puts on the board, every turn of every solver line.
  let sweepOk = 0, turns = 0;
  for (let s = 100; s < 112; s++) {
    let st = E.newGame(s, 0);
    let all = true;
    for (const ch of st.level.solution) {
      const dir = ch === "R" ? "cw" : "ccw";
      const trace = E.resolveTrace(E.rotateGrid(st.grid, dir));
      st = E.rotate(st, dir);
      turns++;
      if (E.gridString(trace.grid) !== E.gridString(st.grid)) all = false;
    }
    if (all) sweepOk++;
  }
  check("12-seed sweep: trace final grid == rotate() grid on every solver turn",
    sweepOk === 12, `${sweepOk}/12 seeds, ${turns} turns checked`);

  // juice timeline: deterministic, sequential, non-overlapping; empty falls
  // produce no step; total == last step's end.
  const rot3 = (() => { // the solver line's 3rd rotation — the collecting one
    let st = E.newGame(SEED, 0);
    st = E.rotate(st, "cw"); st = E.rotate(st, "cw");
    return E.rotateGrid(st.grid, "cw");
  })();
  const trace3 = E.resolveTrace(rot3);
  const tl = J.timeline(trace3.phases), tl2 = J.timeline(trace3.phases);
  let mono = tl.steps.length > 0, prevEnd = 0;
  for (const s of tl.steps) { if (!(s.t0 >= prevEnd && s.t1 >= s.t0)) mono = false; prevEnd = s.t1; }
  check("juice timeline: sequential + non-overlapping, total == last end, deterministic",
    mono && tl.total === prevEnd && JSON.stringify(tl) === JSON.stringify(tl2) &&
    tl.steps.length === trace3.phases.filter(p => p.type !== "fall" || p.moves.length).length,
    `${tl.steps.length} steps over ${tl.total}ms`);

  // juice cue schedule on the collecting rotation: land/collect cues in
  // chronological order, chains rising — the exact log the shell must emit.
  const cueSeq = [];
  for (const s of tl.steps) for (const c of s.cues) cueSeq.push(J.cueFor(c.name, c.chain).key);
  const chainsInSeq = tl.steps.filter(s => s.phase.type === "collect").map(s => s.phase.chain);
  check("cue schedule of the collecting rotation: collects present, chain x2 keyed",
    cueSeq.includes("collect") && cueSeq.includes("collect@x2") &&
    chainsInSeq.every((c, i) => i === 0 || c >= chainsInSeq[i - 1]),
    `[${cueSeq.join(",")}] chains=[${chainsInSeq.join(",")}]`);

  // cue table: every named cue resolves; collect pitch rises with the chain
  // and caps; unknown names are null.
  const names = ["rotate", "land", "collect", "win", "lose", "undo"];
  const allCues = names.every(n => { const c = J.cueFor(n, 0); return c && c.wave && c.f0 > 0 && c.dur > 0; });
  const c1 = J.cueFor("collect", 1), c2 = J.cueFor("collect", 2), c3 = J.cueFor("collect", 3);
  const cCap = J.cueFor("collect", J.CHAIN_CAP + 1), cOver = J.cueFor("collect", J.CHAIN_CAP + 5);
  check("cue table: all 6 cues resolve, collect pitch rises per chain and caps, unknown -> null",
    allCues && c2.f0 > c1.f0 && c3.f0 > c2.f0 && cCap.f0 === cOver.f0 &&
    c1.key === "collect" && c2.key === "collect@x2" && J.cueFor("nope", 0) === null,
    `f0: x1=${c1.f0} x2=${c2.f0} x3=${c3.f0} cap=${cCap.f0} (juice v${J.VERSION})`);
}

// ------------------------------------------------------------------- verdict
if (failures) {
  console.error(`SMOKE FAIL: ${failures} assertion(s) red`);
  process.exit(1);
}
console.log(`SMOKE PASS: all assertions green (engine v${E.VERSION}, seed=${SEED}, solution="${g0.level.solution}")`);
