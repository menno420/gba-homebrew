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
 *  12. new cell types (slice 4): one-way grates pass exactly downward and
 *      turn with the cavern, ice slips deterministically (left first) and
 *      drops its cargo, locked gems never group and free on an adjacent
 *      collect (cascading through the freed gem), settleMoves/resolveTrace
 *      stay exact on kitchen-sink grids, and deep-level generation places
 *      the new cells while levels 0-3 draw the identical RNG stream
 *  13. level packs (slice 5): difficulty() is the pinned pure rating
 *      (par dominates, slack breaks ties downward), curatePack() is
 *      deterministic and REPRODUCES the engine's pinned PACKS data
 *      byte-for-byte (the honest-curation proof), every curated entry is
 *      provably winnable by its own solver line, entries are sorted
 *      hardest-first, and the pinned pars show the packs are genuinely
 *      harder than the baseline caverns
 *  14. shareable line (arc 2, cut 1): encodeShare/decodeShare round-trip
 *      (level=0 omitted, non-zero level carried), line validation +
 *      normalization (junk/glyphs stripped, 64 cap, illegal line throws on
 *      encode / degrades on decode), decodeShare across all three input
 *      shapes + null on junk, and the load-bearing proof — spectate()
 *      reconstructs the sharer's OWN run byte-identically to a live replay
 *      (every step's trace grid == the authoritative grid) across the
 *      seed-42 line and the 12-seed sweep, freezes at a terminal win, and a
 *      share carries only seed+line (the solver's line stays hidden)
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

// ------------------------------------------ 12. new cell types (slice 4)
{
  const J = require(path.join(here, "juice.js"));

  // encoding: the new codes render distinct chars and sit OUTSIDE the gem range
  {
    const g = E.emptyGrid(8);
    g[0][0] = E.ICE; g[0][1] = E.LOCK0 + 1;
    g[0][2] = E.GRATE0; g[0][3] = E.GRATE0 + 1; g[0][4] = E.GRATE0 + 2; g[0][5] = E.GRATE0 + 3;
    const row = E.gridString(g).split("\n")[0];
    check("cell codes: ice/locked/grates serialize as * b ^ > v < and are not gems",
      row.startsWith("*b^>v<") && !E.isGem(E.ICE) && !E.isGem(E.LOCK0) && !E.isGem(E.GRATE0 + 2) &&
      E.isGem(E.GEM0) && E.isGem(E.GEM0 + 7),
      `row0="${row}"`);
  }

  // one-way grate: porous exactly when pointing down, a wall otherwise
  {
    const mk = (o) => { const g = E.emptyGrid(8); g[4][2] = E.GRATE0 + o; g[0][2] = E.STONE; return E.settle(g); };
    const down = mk(2), up = mk(0), right = mk(1), left = mk(3);
    check("grate one-way: stone falls THROUGH a down grate to the floor, rests ON every other orientation",
      down[7][2] === E.STONE && down[4][2] === E.GRATE0 + 2 &&
      up[3][2] === E.STONE && right[3][2] === E.STONE && left[3][2] === E.STONE,
      `down->row7, up/right/left->row3, grate cell intact`);
    const floorGrate = E.emptyGrid(8); floorGrate[7][2] = E.GRATE0 + 2; floorGrate[0][2] = E.STONE;
    const fg = E.settle(floorGrate);
    check("a piece never rests INSIDE a porous grate (floor grate: stone rests above it)",
      fg[6][2] === E.STONE && fg[7][2] === E.GRATE0 + 2);
  }

  // grate arrow turns with the cavern: CW cycles up->right->down->left, 4x = identity
  {
    const g = E.emptyGrid(8); g[3][3] = E.GRATE0 + 2; // down
    const cw = E.rotateGrid(g, "cw"), ccw = E.rotateGrid(g, "ccw");
    const codeIn = (grid) => [].concat(...grid).filter(v => v !== E.EMPTY)[0];
    let four = g; for (let i = 0; i < 4; i++) four = E.rotateGrid(four, "cw");
    check("grate orientation rotates with the cavern (CW down->left, CCW down->right, 4xCW identity)",
      codeIn(cw) === E.GRATE0 + 3 && codeIn(ccw) === E.GRATE0 + 1 &&
      E.gridString(four) === E.gridString(g),
      `cw=${codeIn(cw) - E.GRATE0} ccw=${codeIn(ccw) - E.GRATE0}`);
  }

  // ice: slips off a pile (left before right), cargo drops after it, and the
  // move list nets the slide as one diagonal move that still reconstructs
  {
    const g = E.emptyGrid(8); g[7][3] = E.STONE; g[6][3] = E.ICE; g[5][3] = E.GEM0;
    const s = E.settle(g);
    check("ice slips LEFT off a pile and its cargo drops into the vacated cell",
      s[7][2] === E.ICE && s[6][3] === E.GEM0 && s[7][3] === E.STONE,
      `ice@[7,2] gem@[6,3]`);
    const gr = E.emptyGrid(8); gr[7][2] = E.WALL; gr[7][3] = E.STONE; gr[6][3] = E.ICE;
    check("left blocked -> ice slips RIGHT (deterministic preference)",
      E.settle(gr)[7][4] === E.ICE);
    const boxed = E.emptyGrid(8); boxed[7][2] = E.WALL; boxed[7][4] = E.WALL; boxed[7][3] = E.STONE; boxed[6][3] = E.ICE;
    check("boxed-in ice stays put (no slip without side + diagonal empty)",
      E.settle(boxed)[6][3] === E.ICE);
    const sm = E.settleMoves(g);
    const rebuilt = E.cloneGrid(g);
    for (const mv of sm.moves) rebuilt[mv.from[0]][mv.from[1]] = E.EMPTY;
    for (const mv of sm.moves) rebuilt[mv.to[0]][mv.to[1]] = mv.v;
    check("settleMoves nets a slip as ONE diagonal move; grid == settle, moves reconstruct",
      E.gridString(sm.grid) === E.gridString(s) && E.gridString(rebuilt) === E.gridString(s) &&
      sm.moves.some(m => m.v === E.ICE && m.from[1] !== m.to[1]),
      `moves=${sm.moves.map(m => `[${m.from}]->[${m.to}]`).join(" ")}`);
  }

  // locked gems: never group on their own; an adjacent collect frees one and
  // the freed gem cascades a chain-2 collect
  {
    const solo = E.emptyGrid(8); solo[7][0] = E.LOCK0; solo[7][1] = E.LOCK0; solo[7][2] = E.LOCK0;
    check("three adjacent same-color LOCKED gems never collect",
      E.findGroups(E.settle(solo)).length === 0 && E.resolve(solo).collected === 0);
    const g = E.emptyGrid(8);
    g[7][0] = E.GEM0; g[6][0] = E.GEM0; g[5][0] = E.GEM0;   // red column pops chain 1
    g[7][1] = E.LOCK0 + 1;                                   // locked blue beside it
    g[7][2] = E.GEM0 + 1; g[7][3] = E.GEM0 + 1;              // two free blues waiting
    const res = E.resolve(g);
    check("adjacent collect FREES a locked gem (event carries unlocked: 1)",
      res.events.length === 2 && res.events[0].unlocked === 1 && res.events[0].chain === 1,
      `events=${JSON.stringify(res.events)}`);
    check("the freed gem completes the group -> chain-2 cascade through the lock",
      res.collected === 6 && res.events[1].chain === 2 && res.events[1].color === 1 &&
      res.grid[7][1] === E.EMPTY,
      `collected=${res.collected}`);
    const lockFree = E.emptyGrid(8);
    lockFree[7][0] = E.GEM0; lockFree[6][0] = E.GEM0; lockFree[5][0] = E.GEM0; lockFree[7][5] = E.LOCK0 + 1;
    const far = E.resolve(lockFree);
    check("a NON-adjacent collect leaves the lock intact (no unlocked field on the event)",
      far.grid[7][5] === E.LOCK0 + 1 && far.events.length === 1 && !("unlocked" in far.events[0]));
  }

  // trace exactness + unlock cue on a kitchen-sink grid
  {
    const g = E.emptyGrid(8);
    g[7][0] = E.GEM0; g[6][0] = E.GEM0; g[5][0] = E.GEM0;    // popping reds
    g[7][1] = E.LOCK0 + 1; g[7][2] = E.GEM0 + 1; g[7][3] = E.GEM0 + 1; // lock chain
    g[7][5] = E.STONE; g[6][5] = E.ICE;                      // slipping ice
    g[4][7] = E.GRATE0 + 2; g[0][7] = E.GEM0 + 2;            // porous grate
    const plain = E.resolve(g), trace = E.resolveTrace(g);
    check("resolveTrace ≡ resolve on a kitchen-sink grid (ice + lock + grate + cascade)",
      E.gridString(trace.grid) === E.gridString(plain.grid) &&
      trace.collected === plain.collected &&
      JSON.stringify(trace.events) === JSON.stringify(plain.events),
      `collected=${trace.collected} events=${trace.events.length}`);
    const unlockPhase = trace.phases.find(p => p.type === "collect" && p.unlocked);
    check("the unlocking collect phase carries the freed cells and its grid shows the freed gem",
      !!unlockPhase && unlockPhase.unlocked.length === 1 &&
      unlockPhase.grid[unlockPhase.unlocked[0][0]][unlockPhase.unlocked[0][1]] === E.GEM0 + 1);
    const tl = J.timeline(trace.phases);
    const cueKeys = [];
    for (const s of tl.steps) for (const c of s.cues) cueKeys.push(J.cueFor(c.name, c.chain).key);
    const u = J.cueFor("unlock", 0);
    check("juice: the unlocking collect schedules an 'unlock' cue; the cue table resolves it",
      cueKeys.includes("unlock") && u && u.key === "unlock" && u.f0 > 0 && u.dur > 0,
      `[${cueKeys.join(",")}] (juice v${J.VERSION})`);
  }

  // generation: levels 0-3 place ZERO new cells (identical RNG stream ->
  // every pre-slice-4 pin holds); level 4+ places them, still provably solvable
  {
    const p0 = E.paramsFor(0), p3 = E.paramsFor(3), p4 = E.paramsFor(4);
    check("paramsFor: no new cells before level 4; ice 2 / locks 2 / grates 1 from level 4",
      p0.ice === 0 && p0.locks === 0 && p0.grates === 0 &&
      p3.ice === 0 && p3.locks === 0 && p3.grates === 0 &&
      p4.ice === 2 && p4.locks === 2 && p4.grates === 1);
    const l4 = E.generateLevel(SEED, 4);
    const gs = E.gridString(l4.grid);
    check("(seed 42, level 4): generated cavern contains ice, locked gems and a grate",
      /\*/.test(gs) && /[a-h]/.test(gs) && /[\^>v<]/.test(gs),
      `salt=${l4.salt} quota=${l4.quota} best=${l4.best} solution="${l4.solution}"`);
    check("(seed 42, level 4): the generator's own solver line wins it",
      E.replay(E.newGame(SEED, 4), l4.solution).status === "won");
    let ok4 = 0, worst4 = 0;
    for (let s = 100; s < 112; s++) {
      const lvl = E.generateLevel(s, 4);
      worst4 = Math.max(worst4, lvl.salt);
      if (E.replay(E.newGame(s, 4), lvl.solution).status === "won") ok4++;
    }
    check("12-seed sweep at level 4: every deep cavern is provably winnable with the new cells live",
      ok4 === 12, `${ok4}/12 won, worst salt=${worst4}`);
  }
}

// ------------------------------------------- 13. level packs (slice 5)
{
  // difficulty(): pinned pure truth on the pinned seed-42 level
  const d42 = E.difficulty(g0.level);
  check("difficulty(seed 42): par=3 slack=4 (best 11 - quota 7) score=3995 TRICKY",
    d42.par === 3 && d42.slack === 4 && d42.score === 3995 && d42.label === "TRICKY",
    `par=${d42.par} slack=${d42.slack} score=${d42.score} label=${d42.label}`);

  // label table + the ordering law: par dominates, then LOW slack wins
  const fake = (sol, best, quota) => ({ solution: sol, best, quota });
  const labels = ["RR", "RRR", "RRRR", "RRRRR"].map(s => E.difficulty(fake(s, 10, 6)).label).join(",");
  const parWins = E.difficulty(fake("RRRR", 999 + 6, 6)).score > E.difficulty(fake("RRR", 6, 6)).score;
  const slackBreaks = E.difficulty(fake("RRR", 8, 6)).score > E.difficulty(fake("RRR", 12, 6)).score;
  check("difficulty law: labels MILD/TRICKY/STIFF/CRUEL by par; par dominates; low slack breaks ties",
    labels === "MILD,TRICKY,STIFF,CRUEL" && parWins && slackBreaks, `[${labels}]`);

  // the honest-curation proof: re-running curatePack on each shipped def
  // reproduces the engine's pinned PACKS byte-for-byte (and twice = twice)
  const rerun = E.PACK_DEFS.map(def => E.curatePack(def));
  check("PACKS pin == curatePack(def) re-run, byte-for-byte, for every shipped pack",
    E.PACKS.length === E.PACK_DEFS.length &&
    rerun.every((p, i) => JSON.stringify(p) === JSON.stringify(E.PACKS[i])),
    E.PACKS.map(p => `${p.id}:${p.entries.map(e => e.seed).join("/")}`).join(" · "));
  check("curatePack is deterministic (same def twice -> identical JSON)",
    JSON.stringify(E.curatePack(E.PACK_DEFS[0])) === JSON.stringify(rerun[0]));

  // structure: take honored, seeds inside the scan window, hardest-first
  // (score non-increasing, seed ascending on equal scores)
  let structOk = true;
  E.PACKS.forEach((p, i) => {
    const def = E.PACK_DEFS[i];
    if (p.entries.length !== def.take || p.levelIndex !== def.levelIndex) structOk = false;
    for (let j = 0; j < p.entries.length; j++) {
      const e = p.entries[j];
      if (e.seed < def.scanFrom || e.seed >= def.scanFrom + def.scanCount) structOk = false;
      if (j > 0) {
        const prev = p.entries[j - 1];
        if (e.score > prev.score || (e.score === prev.score && e.seed < prev.seed)) structOk = false;
      }
    }
  });
  check("pack structure: take honored, seeds in the scan window, sorted hardest-first", structOk);

  // every curated entry is provably winnable at its pack's depth, and its
  // stored rating matches a fresh generation of that seed
  let winnable = 0, ratedOk = 0, total = 0;
  for (const p of E.PACKS) {
    for (const e of p.entries) {
      total++;
      const lvl = E.generateLevel(e.seed, p.levelIndex);
      const d = E.difficulty(lvl);
      if (d.par === e.par && d.slack === e.slack && d.score === e.score) ratedOk++;
      if (E.replay(E.newGame(e.seed, p.levelIndex), lvl.solution).status === "won") winnable++;
    }
  }
  check("every pack entry: solver line wins + stored rating == fresh generation",
    winnable === total && ratedOk === total, `${winnable}/${total} winnable, ${ratedOk}/${total} ratings exact`);

  // pinned hardness: the curated pars, hardest-first — visibly above the
  // baseline caverns (seed-42 level 0 par is 3)
  const pars = E.PACKS.map(p => p.entries.map(e => e.par).join(""));
  check("pinned pack pars: granite-gauntlet 766555, deep-cuts 887777 (all > baseline par 3)",
    pars[0] === "766555" && pars[1] === "887777" &&
    E.PACKS.every(p => p.entries.every(e => e.par >= 5)),
    `[${pars.join(" | ")}]`);

  // packById resolves the shipped ids; unknown ids are null
  check("packById: resolves both shipped ids, unknown -> null",
    E.packById("granite-gauntlet") === E.PACKS[0] && E.packById("deep-cuts") === E.PACKS[1] &&
    E.packById("nope") === null);
}

// ----------------------------------- 14. shareable line (arc 2, cut 1)
{
  // encode/decode round-trip on a real cleared line (the seed-42 solver line)
  const shareA = { seed: SEED, levelIndex: 0, line: g0.level.solution };
  const encA = E.encodeShare(shareA);
  const decA = E.decodeShare(encA);
  check("encodeShare: canonical fragment, level=0 omitted",
    encA === `seed=${SEED}&replay=${g0.level.solution}`, encA);
  check("decodeShare(encodeShare(x)) round-trips seed/level/line",
    decA && decA.seed === (SEED >>> 0) && decA.levelIndex === 0 && decA.line === g0.level.solution,
    JSON.stringify(decA));

  // level > 0 is carried; encode/decode round-trips it too
  const encB = E.encodeShare({ seed: 7, levelIndex: 4, line: "LRRL" });
  const decB = E.decodeShare(encB);
  check("encodeShare/decodeShare carry a non-zero level",
    encB === "seed=7&level=4&replay=LRRL" &&
    decB.seed === 7 && decB.levelIndex === 4 && decB.line === "LRRL", encB);

  // isReplayLine + normalizeLine: legal core kept, junk/glyphs stripped, cap held
  check("isReplayLine: accepts LR-only, rejects junk / over-cap / non-strings",
    E.isReplayLine("") && E.isReplayLine("LRRL") &&
    !E.isReplayLine("LxR") && !E.isReplayLine("lr") &&
    !E.isReplayLine("L".repeat(65)) && !E.isReplayLine(42) && !E.isReplayLine(null));
  check("normalizeLine: uppercases, drops non-L/R, caps at 64",
    E.normalizeLine("l r→R l") === "LRRL" &&
    E.normalizeLine("♞RR♞L") === "RRL" &&
    E.normalizeLine("R".repeat(100)).length === 64);
  check("encodeShare throws on an illegal line (never silently corrupts a share)",
    (() => { try { E.encodeShare({ seed: 1, line: "LXR" }); return false; } catch (e) { return true; } })());

  // decodeShare accepts all three input shapes and returns null on junk
  const fromParams = E.decodeShare(new URLSearchParams("seed=99&replay=RR&level=2"));
  const fromBag = E.decodeShare({ seed: 99, level: 2, replay: "RR" });
  const fromLead = E.decodeShare("?seed=99&level=2&replay=RR");
  check("decodeShare: URLSearchParams, plain bag, and ?-led string all agree",
    JSON.stringify(fromParams) === JSON.stringify(fromBag) &&
    JSON.stringify(fromBag) === JSON.stringify(fromLead) &&
    fromParams.seed === 99 && fromParams.levelIndex === 2 && fromParams.line === "RR",
    JSON.stringify(fromParams));
  check("decodeShare: null when seed or replay is missing, or on a malformed escape",
    E.decodeShare("seed=5") === null && E.decodeShare("replay=RR") === null &&
    E.decodeShare("") === null && E.decodeShare(123) === null &&
    E.decodeShare("seed=%zz&replay=RR") === null);
  check("decodeShare: a URL-junk replay degrades to its legal core, not a throw",
    E.decodeShare("seed=1&replay=Rx-L").line === "RL");

  // the load-bearing proof: spectate() reconstructs the sharer's OWN run
  // byte-identically to a live replay, and every step's trace grid matches the
  // authoritative post-rotation grid
  const spec = E.spectate(SEED, 0, g0.level.solution);
  const live = E.replay(E.newGame(SEED, 0), g0.level.solution);
  const gridsAgree = spec.steps.every(s => E.gridString(s.trace.grid) === E.gridString(s.state.grid));
  check("spectate: final state == live replay, every step trace grid == authoritative grid",
    E.gridString(spec.final.grid) === E.gridString(live.grid) &&
    spec.final.status === live.status && spec.final.collected === live.collected &&
    spec.final.status === "won" && spec.clean && gridsAgree && spec.consumed === g0.level.solution.length,
    `status=${spec.final.status} consumed=${spec.consumed}/${g0.level.solution.length}`);

  // a won cavern freezes: extra rotations past the win are ignored (consumed
  // stops at the winning line's length, not the padded line's)
  const padded = g0.level.solution + "LRLR";
  const specPad = E.spectate(SEED, 0, padded);
  check("spectate: stops at terminal — trailing rotations after a win are ignored",
    specPad.consumed === g0.level.solution.length &&
    E.gridString(specPad.final.grid) === E.gridString(spec.final.grid));

  // determinism + the OWN-line-only guarantee across the 12-seed sweep: every
  // seed's solver line spectates identically to a live replay, and no share
  // function ever emits the solver's hidden line for you
  let sweepOk = 0;
  for (let s = SEED; s < SEED + 12; s++) {
    const lvl = E.newGame(s, 0).level;
    const sp = E.spectate(s, 0, lvl.solution);
    const lv = E.replay(E.newGame(s, 0), lvl.solution);
    const enc = E.encodeShare({ seed: s, levelIndex: 0, line: lvl.solution });
    // the encoded share contains ONLY what the sharer chose to send (seed +
    // their line) — decoding it never reveals more than went in
    const dec = E.decodeShare(enc);
    if (E.gridString(sp.final.grid) === E.gridString(lv.grid) &&
        sp.final.status === "won" && dec.line === lvl.solution &&
        !enc.includes("solution")) sweepOk++;
  }
  check("spectate == live replay across the 12-seed sweep; a share carries only seed+line",
    sweepOk === 12, `${sweepOk}/12`);
}

// ----------------------------------- 15. solver hints (arc 2, cut 2)
{
  const par0 = E.par(g0.level);

  // the hint from a pristine board is exactly the solver's first stored move —
  // hintFrom re-derives it via the SAME BFS `solve` uses, it doesn't read the line
  check("hintFrom(pristine) == level.solution[0] (agrees with the stored solver line)",
    E.hintFrom(E.newGame(SEED, 0)) === g0.level.solution.charAt(0),
    `${E.hintFrom(E.newGame(SEED, 0))} vs ${g0.level.solution.charAt(0)}`);

  // never reveal a hint for a terminal or malformed board
  check("hintFrom: null on won / lost / gridless / junk states",
    E.hintFrom({ status: "won" }) === null &&
    E.hintFrom({ status: "lost" }) === null &&
    E.hintFrom({ status: "playing" }) === null &&   // no grid/quota -> defensive null
    E.hintFrom(null) === null && E.hintFrom(undefined) === null);

  // following the hint move-by-move ALWAYS reaches 'won', within budget, in
  // exactly par turns (each hint is the first step of a shortest winning
  // continuation, so greedily following stays optimal)
  function followHints(seed) {
    let st = E.newGame(seed, 0), guard = 0, taken = "";
    while (st.status === "playing") {
      const h = E.hintFrom(st);
      if (h !== "L" && h !== "R") return { ok: false, taken, st };
      taken += h;
      st = E.rotate(st, h === "R" ? "cw" : "ccw");
      if (++guard > st.budget + 2) return { ok: false, taken, st };
    }
    return { ok: st.status === "won" && st.used <= st.budget, taken, st };
  }
  const f0 = followHints(SEED);
  check("hintFrom: following hints from seed 42 wins within budget, in exactly par turns",
    f0.ok && f0.st.used === par0 && f0.taken.length === par0 && E.hintFrom(f0.st) === null,
    `used=${f0.st.used} par=${par0} taken=${f0.taken} terminalHint=${E.hintFrom(f0.st)}`);

  let hintSweep = 0;
  for (let s = SEED; s < SEED + 12; s++) {
    const f = followHints(s);
    const p = E.par(E.newGame(s, 0).level);
    if (f.ok && f.st.used === p && f.taken.length === p && E.hintFrom(f.st) === null) hintSweep++;
  }
  check("hintFrom: hint-follow wins in exactly par across the 12-seed sweep",
    hintSweep === 12, `${hintSweep}/12`);

  // OFF-LINE HONESTY: after a deliberate NON-solver first move, hintFrom re-solves
  // from the ACTUAL board — either pointing along a fresh shortest win (following
  // it still wins in the remaining budget) or honestly returning null when the
  // detour made the cavern unwinnable. It never parrots the stale stored line.
  {
    const start = E.newGame(SEED, 0);
    const off = g0.level.solution.charAt(0) === "R" ? "ccw" : "cw";   // opposite of the solver
    const s1 = E.rotate(start, off);
    const offHint = E.hintFrom(s1);
    const need1 = s1.quota - s1.collected;
    const rec = E.search(s1.grid, s1.budget - s1.used).records.find(r => r.collected >= need1);
    if (rec) {
      let st = s1, guard = 0;
      while (st.status === "playing") {
        const h = E.hintFrom(st);
        if (h !== "L" && h !== "R") break;
        st = E.rotate(st, h === "R" ? "cw" : "ccw");
        if (++guard > st.budget + 2) break;
      }
      check("hintFrom: after an off-line detour it re-solves the ACTUAL board and still wins in budget",
        offHint === rec.path.charAt(0) && st.status === "won" && st.used <= st.budget,
        `offHint=${offHint} used=${st.used} par=${par0}`);
    } else {
      check("hintFrom: an off-line detour that kills the cavern returns an honest null",
        offHint === null, `offHint=${offHint}`);
    }
  }

  // spend-gating: hintedGrade folds hints into `used` like over-par turns, so a
  // par run leaning on 2 hints grades GOOD not PERFECT; 0 hints == plain grade
  check("hintedGrade: pinned spend-gating truth table (a hint dings the card like an over-par turn)",
    E.hintedGrade(par0, par0, 0).label === "PERFECT" && E.hintedGrade(par0, par0, 0).diff === 0 &&
    E.hintedGrade(par0, par0, 1).label === "GREAT"   && E.hintedGrade(par0, par0, 1).diff === 1 &&
    E.hintedGrade(par0, par0, 2).label === "GOOD"    && E.hintedGrade(par0, par0, 2).diff === 2 &&
    E.hintedGrade(par0, par0, 3).label === "CLEARED" && E.hintedGrade(par0, par0, 3).diff === 3 &&
    JSON.stringify(E.hintedGrade(par0 + 4, par0, 0)) === JSON.stringify(E.grade(par0 + 4, par0)) &&
    JSON.stringify(E.hintedGrade(par0, par0, -5)) === JSON.stringify(E.grade(par0, par0)),
    JSON.stringify(E.hintedGrade(par0, par0, 2)));
}

// ----------------------------- 16. undo×par deception (arc 2, cut 3)
{
  const par0 = E.par(g0.level);
  // pinned truth table: [undos, used, par, want deception scalar, want label]
  const rows = [
    [0, par0,     par0, 0,  "clean-honest"],
    [6, par0,     par0, 12, "HARD-deceptive"],
    [0, par0 + 2, par0, 0,  "medium-honest"],
    [2, par0,     par0, 4,  "medium-deceptive"],
    [3, par0 + 3, par0, 3,  "HARD-tricky"],
    [1, par0,     par0, 2,  "medium-tricky"],
    [1, par0 + 1, par0, 1,  "medium-tricky"],
  ];
  for (const [undos, used, p, wantD, wantLabel] of rows) {
    check(`deception(${undos},${used},${p})==${wantD}`, E.deception(undos, used, p) === wantD, `got ${E.deception(undos, used, p)}`);
    check(`deceptionLabel(${undos},${used},${p})=="${wantLabel}"`, E.deceptionLabel(undos, used, p) === wantLabel, `got "${E.deceptionLabel(undos, used, p)}"`);
  }
  const a = E.deception(4, par0 + 1, par0), b = E.deception(4, par0 + 1, par0);
  check("deception deterministic", a === b, `${a} vs ${b}`);
  let mono = true, prev = -1;
  for (let k = 0; k <= 8; k++) { const v = E.deception(k, par0, par0); if (v < prev) mono = false; prev = v; }
  check("deception monotone in undos", mono, "");
  check("deception floors at 0", E.deception(-3, par0, par0) === 0 && E.deception(0, par0 + 9, par0) === 0, "");
}

// ------------------------------------------------------------------- verdict
if (failures) {
  console.error(`SMOKE FAIL: ${failures} assertion(s) red`);
  process.exit(1);
}
console.log(`SMOKE PASS: all assertions green (engine v${E.VERSION}, seed=${SEED}, solution="${g0.level.solution}")`);
