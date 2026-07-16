/* Tiltstone engine — ALL game rules, pure and headless.
 *
 * No DOM, no canvas, no wall-clock, no I/O: every function is a pure
 * function of its arguments, and after level generation there is NO
 * randomness at all — rotation, gravity, merging and win/lose are fully
 * deterministic, so the same seed + the same rotation sequence is the
 * identical game everywhere (browser, Node, CI).
 *
 * Dual-mode loading:
 *   - browser: <script src="engine.js"> -> window.TiltstoneEngine
 *   - Node:    const E = require("./engine.js")
 *
 * Cell encoding (integers):
 *   0            empty
 *   1            wall  (fixed to the cavern; rotates with it, never falls)
 *   2            stone (falls, never merges — dead weight and plug)
 *   3 + c        gem of color c (0..colors-1); falls, 3+ orthogonally
 *                adjacent same-color gems collect after every settle
 *   11           ice   (slice 4: falls, never merges, and SLIDES — resting
 *                ice with an empty side + empty diagonal-below slips off,
 *                left before right, then keeps falling)
 *   12 + c       locked gem of color c (slice 4: falls like dead weight,
 *                never joins a group; a collect popping orthogonally
 *                adjacent breaks the lock and frees a normal gem)
 *   20 + o       one-way grate, orientation o (0=up 1=right 2=down 3=left;
 *                slice 4: fixed like a wall and rotates its arrow with the
 *                cavern; pieces fall THROUGH it only while it points down,
 *                and rest ON it otherwise)
 *
 * A level is generated from (seed, levelIndex) and is PROVABLY SOLVABLE:
 * generation runs a breadth-first solver over the rotation tree (branching
 * factor 2, depth <= budget) and only returns a layout whose quota it can
 * reach itself. The winning line is kept on the level object (`solution`)
 * as proof material — the UI never shows it.
 */
"use strict";

(function (root, factory) {
  var api = factory();
  if (typeof module !== "undefined" && module.exports) module.exports = api;
  else root.TiltstoneEngine = api;
})(typeof self !== "undefined" ? self : this, function () {

  var VERSION = "1.7.0";
  var SIZE = 8;          // grid is SIZE x SIZE (square — rotation-safe)
  var EMPTY = 0, WALL = 1, STONE = 2, GEM0 = 3;
  var ICE = 11;          // slice 4 — first code past the 8 reserved gem slots
  var LOCK0 = 12;        // slice 4 — locked gem of color c is LOCK0 + c
  var GRATE0 = 20;       // slice 4 — grate orientation o is GRATE0 + o
  var MERGE_MIN = 3;     // orthogonal group size that collects
  var MAX_SALT = 64;     // generation re-tries before giving up (never hit in practice)
  var MIN_BEST = 6;      // a layout must offer at least this many collectible gems

  // Cell-class helpers (slice 4). Gems are exactly the 8 reserved codes
  // 3..10, so every pre-slice-4 predicate keeps its old meaning on old
  // content; the new codes live entirely outside that range.
  function isGem(v)    { return v >= GEM0 && v < ICE; }
  function isLocked(v) { return v >= LOCK0 && v < LOCK0 + 8; }
  function isGrate(v)  { return v >= GRATE0 && v < GRATE0 + 4; }
  function isFixedCell(v) { return v === WALL || isGrate(v); }
  function gratePasses(v) { return v === GRATE0 + 2; } // arrow points down

  // ---------------------------------------------------------------- RNG --

  // mulberry32 — tiny, seedable, good-enough PRNG. Used ONLY at level gen.
  function mulberry32(a) {
    a = a >>> 0;
    return function () {
      a |= 0; a = (a + 0x6D2B79F5) | 0;
      var t = Math.imul(a ^ (a >>> 15), 1 | a);
      t = (t + Math.imul(t ^ (t >>> 7), 61 | t)) ^ t;
      return ((t ^ (t >>> 14)) >>> 0) / 4294967296;
    };
  }

  function mixSeed(seed, levelIndex, salt) {
    var h = seed >>> 0;
    h = (h ^ Math.imul(levelIndex + 1, 0x9E3779B9)) >>> 0;
    h = (h ^ Math.imul(salt + 1, 0x85EBCA6B)) >>> 0;
    h = Math.imul(h ^ (h >>> 16), 0x45D9F3B) >>> 0;
    h = (h ^ (h >>> 13)) >>> 0;
    return h;
  }

  // Daily seed: pure function of a UTC date string "YYYY-MM-DD" -> integer
  // (e.g. 2026-07-13 -> 20260713). Callers pass the date in; the engine
  // never reads the clock itself.
  function dailySeed(utcDateStr) {
    var m = /^(\d{4})-(\d{2})-(\d{2})$/.exec(utcDateStr);
    if (!m) throw new Error("dailySeed wants 'YYYY-MM-DD', got: " + utcDateStr);
    return (+m[1]) * 10000 + (+m[2]) * 100 + (+m[3]);
  }

  // --------------------------------------------------------- grid basics --

  function emptyGrid(n) {
    var g = new Array(n);
    for (var r = 0; r < n; r++) { g[r] = new Array(n); for (var c = 0; c < n; c++) g[r][c] = EMPTY; }
    return g;
  }

  function cloneGrid(g) {
    var n = g.length, out = new Array(n);
    for (var r = 0; r < n; r++) out[r] = g[r].slice();
    return out;
  }

  // Index by cell code: gems A.., stone o, wall #; slice 4 adds ice *,
  // locked gems a.. (lowercase mirror of their color), grates ^ > v <.
  var CHARS = ".#oABCDEFGH*abcdefgh^>v<";
  function cellChar(v) { return CHARS[v] || "?"; }

  // Canonical ASCII serialization — the pinning/hashing surface for proofs.
  function gridString(g) {
    var rows = [];
    for (var r = 0; r < g.length; r++) {
      var s = "";
      for (var c = 0; c < g.length; c++) s += cellChar(g[r][c]);
      rows.push(s);
    }
    return rows.join("\n");
  }

  // Rotate the WHOLE cavern (walls included) 90 degrees. A grate's arrow is
  // part of the cavern, so its orientation code turns with it (slice 4).
  function rotateGrid(g, dir) {
    var n = g.length, out = emptyGrid(n);
    for (var r = 0; r < n; r++)
      for (var c = 0; c < n; c++) {
        var v = dir === "cw" ? g[n - 1 - c][r] : g[c][n - 1 - r];
        if (isGrate(v)) v = GRATE0 + ((v - GRATE0 + (dir === "cw" ? 1 : 3)) % 4);
        out[r][c] = v;
      }
    return out;
  }

  // Gravity: movable cells (stone, gems, ice, locked gems) fall straight
  // down within their column until the floor, a fixed cell, or another
  // piece stops them. Walls and grates stay put; a DOWN-pointing grate is
  // porous (pieces fall through it and never rest in it), every other
  // orientation blocks like a wall. Then resting ice slips: an ice cell
  // whose side + diagonal-below are both empty slides off (left before
  // right, deterministic) and keeps falling; anything it was carrying
  // drops after it. Iterates to a fixed point — every slip moves ice
  // strictly downward, so it terminates. Pure; returns a NEW grid.
  //
  // settleCore also tracks each piece's NET movement (original cell ->
  // final cell) so settleMoves can hand the shell one move per piece —
  // for old content (no ice/grates) the move list is exactly the
  // pre-slice-4 one, in the same order.

  // One straight-drop pass — the v1.2.0 column walk extended with grates.
  // `orig` maps "r,c" of a CURRENT piece position to its original [r,c];
  // pass null on the first pass (pieces start at their own cells).
  function straightPass(src, orig) {
    var n = src.length, out = emptyGrid(n), norig = {};
    for (var c = 0; c < n; c++) {
      for (var r = 0; r < n; r++) if (isFixedCell(src[r][c])) out[r][c] = src[r][c];
      var write = n - 1;
      for (var r2 = n - 1; r2 >= 0; r2--) {
        var v = src[r2][c];
        if (isFixedCell(v)) {
          if (!gratePasses(v)) write = r2 - 1;      // wall / blocking grate: rest on top
          else if (write === r2) write = r2 - 1;    // porous, but never a resting slot
          continue;
        }
        if (v >= STONE) {
          while (write >= 0 && isFixedCell(out[write][c])) write--;
          out[write][c] = v;
          norig[write + "," + c] = (orig && orig[r2 + "," + c]) || [r2, c];
          write--;
        }
      }
    }
    return { grid: out, orig: norig };
  }

  // Where a piece entering column c at row r comes to rest: the deepest
  // EMPTY cell reachable straight down through empties and porous grates.
  function dropRow(grid, r, c) {
    var n = grid.length, rest = r;
    for (var t = r + 1; t < n; t++) {
      var v = grid[t][c];
      if (v === EMPTY) { rest = t; continue; }
      if (gratePasses(v)) continue;   // falls through, cannot rest inside
      break;
    }
    return rest;
  }

  function canSlip(grid, r, c, d) {
    var n = grid.length, cc = c + d;
    return cc >= 0 && cc < n && r + 1 < n &&
      grid[r][cc] === EMPTY && grid[r + 1][cc] === EMPTY;
  }

  function settleCore(g) {
    var n = g.length;
    var pass = straightPass(g, null);
    var grid = pass.grid, orig = pass.orig;
    for (;;) {
      var slid = false;
      for (var r = n - 2; r >= 0 && !slid; r--) {       // bottom-up scan
        for (var c = 0; c < n && !slid; c++) {
          if (grid[r][c] !== ICE) continue;
          var d = canSlip(grid, r, c, -1) ? -1 : canSlip(grid, r, c, 1) ? 1 : 0;
          if (!d) continue;
          var from = orig[r + "," + c];
          delete orig[r + "," + c];
          grid[r][c] = EMPTY;
          var land = dropRow(grid, r, c + d);
          grid[land][c + d] = ICE;
          orig[land + "," + (c + d)] = from;
          slid = true;
        }
      }
      if (!slid) break;
      var again = straightPass(grid, orig);             // whatever it carried drops
      grid = again.grid; orig = again.orig;
    }
    // Emit net moves in the legacy order: source column ascending, source
    // row bottom-up — byte-identical to the v1.2.0 list on old content.
    var moves = [];
    for (var rr = 0; rr < n; rr++)
      for (var cc2 = 0; cc2 < n; cc2++) {
        var from2 = orig[rr + "," + cc2];
        if (from2 && (from2[0] !== rr || from2[1] !== cc2))
          moves.push({ v: grid[rr][cc2], from: [from2[0], from2[1]], to: [rr, cc2] });
      }
    moves.sort(function (a, b) { return a.from[1] - b.from[1] || b.from[0] - a.from[0]; });
    return { grid: grid, moves: moves };
  }

  function settle(g) { return settleCore(g).grid; }

  // Find all orthogonal same-color gem groups of size >= MERGE_MIN.
  // Returns [{color, cells:[[r,c],...]}, ...] in deterministic scan order.
  function findGroups(g) {
    var n = g.length, seen = emptyGrid(n), groups = [];
    for (var r = 0; r < n; r++) {
      for (var c = 0; c < n; c++) {
        var v = g[r][c];
        if (!isGem(v) || seen[r][c]) continue;  // locked gems / ice never group
        var stack = [[r, c]], cells = [];
        seen[r][c] = 1;
        while (stack.length) {
          var p = stack.pop(); cells.push(p);
          var nb = [[p[0] - 1, p[1]], [p[0] + 1, p[1]], [p[0], p[1] - 1], [p[0], p[1] + 1]];
          for (var i = 0; i < 4; i++) {
            var rr = nb[i][0], cc = nb[i][1];
            if (rr >= 0 && rr < n && cc >= 0 && cc < n && !seen[rr][cc] && g[rr][cc] === v) {
              seen[rr][cc] = 1; stack.push([rr, cc]);
            }
          }
        }
        if (cells.length >= MERGE_MIN) groups.push({ color: v - GEM0, cells: cells });
      }
    }
    return groups;
  }

  // A collect popping orthogonally adjacent to a locked gem breaks the
  // lock (slice 4): the cell becomes a normal gem of its color, in place,
  // and the NEXT cascade round can group through it. Mutates `grid`;
  // returns the freed cells in deterministic (group-cell, N/S/W/E) order.
  function unlockAround(grid, cells) {
    var n = grid.length, freed = [];
    for (var j = 0; j < cells.length; j++) {
      var r = cells[j][0], c = cells[j][1];
      var nb = [[r - 1, c], [r + 1, c], [r, c - 1], [r, c + 1]];
      for (var i = 0; i < 4; i++) {
        var rr = nb[i][0], cc = nb[i][1];
        if (rr < 0 || rr >= n || cc < 0 || cc >= n) continue;
        var v = grid[rr][cc];
        if (isLocked(v)) { grid[rr][cc] = GEM0 + (v - LOCK0); freed.push([rr, cc]); }
      }
    }
    return freed;
  }

  // Settle, then repeatedly collect groups and re-settle (cascade chains).
  // Pure: takes a grid, returns { grid, collected, events }. A collect
  // event gains `unlocked: k` ONLY when it freed k locked gems, so event
  // objects on lock-free content stay byte-identical to pre-slice-4.
  function resolve(g) {
    var grid = settle(g), collected = 0, events = [], chain = 0;
    for (;;) {
      var groups = findGroups(grid);
      if (!groups.length) break;
      chain++;
      for (var i = 0; i < groups.length; i++) {
        var grp = groups[i];
        collected += grp.cells.length;
        var ev = { type: "collect", color: grp.color, size: grp.cells.length, chain: chain };
        for (var j = 0; j < grp.cells.length; j++) grid[grp.cells[j][0]][grp.cells[j][1]] = EMPTY;
        var freed = unlockAround(grid, grp.cells);
        if (freed.length) ev.unlocked = freed.length;
        events.push(ev);
      }
      grid = settle(grid);
    }
    return { grid: grid, collected: collected, events: events };
  }

  // ------------------------------------------------- trace (slice 3 juice) --

  // settle() plus a per-piece move list — the animation surface. Shares
  // settleCore with settle, so the returned grid is BYTE-IDENTICAL to
  // settle(g) (asserted in the smoke); `moves` records every piece that
  // actually moved as { v, from:[r,c], to:[r,c] } — the NET movement, so a
  // slipping ice gets ONE move that may change column. Froms are all
  // distinct and tos are all distinct, so clearing every `from` then
  // writing every `to` on a copy of the input reconstructs the settled
  // grid (also asserted). Pure.
  function settleMoves(g) { return settleCore(g); }

  // resolve() plus the ordered intermediate story — what the shell tweens.
  // Returns { grid, collected, events, phases } where grid/collected/events
  // are exactly resolve(g)'s (asserted in the smoke; same loop, same scan
  // order) and phases is the render script:
  //   { type:'fall',    moves, grid }                        — gravity step
  //   { type:'collect', color, size, chain, cells, grid }    — one group pops
  // Every phase carries the grid AFTER it applies; phases[0] is always the
  // initial fall (possibly zero moves) and the last phase's grid is the
  // final grid. Pure — no DOM, no clock; timing lives in juice.js.
  function resolveTrace(g) {
    var st = settleMoves(g);
    var grid = st.grid, collected = 0, events = [], chain = 0;
    var phases = [{ type: "fall", moves: st.moves, grid: cloneGrid(grid) }];
    for (;;) {
      var groups = findGroups(grid);
      if (!groups.length) break;
      chain++;
      for (var i = 0; i < groups.length; i++) {
        var grp = groups[i];
        collected += grp.cells.length;
        var ev = { type: "collect", color: grp.color, size: grp.cells.length, chain: chain };
        for (var j = 0; j < grp.cells.length; j++) grid[grp.cells[j][0]][grp.cells[j][1]] = EMPTY;
        var freed = unlockAround(grid, grp.cells);   // same rule + order as resolve()
        if (freed.length) ev.unlocked = freed.length;
        events.push(ev);
        var phase = { type: "collect", color: grp.color, size: grp.cells.length, chain: chain, cells: grp.cells, grid: cloneGrid(grid) };
        if (freed.length) phase.unlocked = freed;    // grid above already shows the freed gems
        phases.push(phase);
      }
      var again = settleMoves(grid);
      grid = again.grid;
      phases.push({ type: "fall", moves: again.moves, grid: cloneGrid(grid) });
    }
    return { grid: grid, collected: collected, events: events, phases: phases };
  }

  // ------------------------------------------------------------- solver --

  // Breadth-first search over rotation sequences up to `budget` deep.
  // Branching factor 2 ('L' = ccw, 'R' = cw) -> at most 2^budget sims, with
  // duplicate-state pruning. Returns:
  //   { best: max gems collectible within budget,
  //     paths: [{path, collected, depth}, ...] in BFS (shortest-first) order }
  function search(grid, budget) {
    var startKey = gridString(grid) + "|0";
    var visited = {}; visited[startKey] = true;
    var frontier = [{ grid: grid, collected: 0, path: "" }];
    var best = 0, records = [];
    for (var depth = 1; depth <= budget; depth++) {
      var next = [];
      for (var i = 0; i < frontier.length; i++) {
        var node = frontier[i];
        var dirs = ["L", "R"];
        for (var d = 0; d < 2; d++) {
          var rot = rotateGrid(node.grid, dirs[d] === "R" ? "cw" : "ccw");
          var res = resolve(rot);
          var collected = node.collected + res.collected;
          var key = gridString(res.grid) + "|" + collected;
          if (visited[key]) continue;
          visited[key] = true;
          var path = node.path + dirs[d];
          if (collected > best) best = collected;
          if (collected > 0) records.push({ path: path, collected: collected, depth: depth });
          next.push({ grid: res.grid, collected: collected, path: path });
        }
      }
      frontier = next;
    }
    return { best: best, records: records };
  }

  // Shortest rotation line that reaches `quota` gems on this level.
  // (Re-runs the BFS; generation stores the result so callers rarely need to.)
  function solve(level) {
    var found = search(level.grid, level.budget);
    for (var i = 0; i < found.records.length; i++)
      if (found.records[i].collected >= level.quota) return found.records[i].path;
    return null;
  }

  // ------------------------------------------------------- par + grading --

  // Par = the length of the SHORTEST winning rotation line for this level.
  // `generateLevel` stores exactly that: `search` pushes records in BFS
  // (shortest-first) order and generation keeps the first record meeting the
  // quota, so `level.solution` is minimal-depth by construction — par is its
  // length. Pure; never reveals the line itself, only how long it is.
  function par(level) {
    return level && level.solution ? level.solution.length : null;
  }

  // Grade a finished level: how many turns over par did the win take?
  // Pure integer mapping — par is a floor (the solver's shortest line), so
  // diff is never negative on a real win; clamped defensively anyway.
  //   0 over -> PERFECT, 1 -> GREAT, 2 -> GOOD, 3+ -> CLEARED
  function grade(used, parTurns) {
    var diff = Math.max(0, (used | 0) - (parTurns | 0));
    var label = diff === 0 ? "PERFECT" : diff === 1 ? "GREAT" : diff === 2 ? "GOOD" : "CLEARED";
    return { diff: diff, label: label };
  }

  // --------------------------------------------------------- generation --

  // Level parameters ramp gently with levelIndex (colors 3 -> 4 at level 3;
  // the slice-4 cell types enter at level 4 — earlier levels place ZERO of
  // them, so their generation draws the exact pre-slice-4 RNG sequence and
  // every pinned level stays byte-identical).
  function paramsFor(levelIndex) {
    return {
      colors: levelIndex >= 3 ? 4 : 3,
      gemsPerColor: 5 + (levelIndex >= 2 ? 1 : 0),
      stones: 4,
      walls: 6 + (levelIndex % 3),
      budget: 10,
      ice: levelIndex >= 4 ? 2 : 0,
      locks: levelIndex >= 4 ? 2 : 0,
      grates: levelIndex >= 4 ? 1 : 0
    };
  }

  // Deterministic level from (seed, levelIndex). Internally tries salted
  // layouts until one (a) starts merge-free after the initial settle and
  // (b) the BFS solver proves >= MIN_BEST gems collectible within budget.
  function generateLevel(seed, levelIndex) {
    levelIndex = levelIndex | 0;
    var P = paramsFor(levelIndex);
    for (var salt = 0; salt < MAX_SALT; salt++) {
      var rng = mulberry32(mixSeed(seed, levelIndex, salt));
      var grid = emptyGrid(SIZE);
      var free = [];
      for (var r = 0; r < SIZE; r++) for (var c = 0; c < SIZE; c++) free.push([r, c]);
      function take() { // pull a random still-free cell
        var i = Math.floor(rng() * free.length);
        var cell = free.splice(i, 1)[0];
        return cell;
      }
      for (var w = 0; w < P.walls; w++) { var p = take(); grid[p[0]][p[1]] = WALL; }
      for (var s = 0; s < P.stones; s++) { var q = take(); grid[q[0]][q[1]] = STONE; }
      for (var col = 0; col < P.colors; col++)
        for (var gph = 0; gph < P.gemsPerColor; gph++) { var t = take(); grid[t[0]][t[1]] = GEM0 + col; }
      // Slice-4 cells LAST, so levels with zero of them (0..3) consume the
      // identical RNG stream as before.
      for (var ic = 0; ic < P.ice; ic++) { var pi = take(); grid[pi[0]][pi[1]] = ICE; }
      for (var lk = 0; lk < P.locks; lk++) { var pl = take(); grid[pl[0]][pl[1]] = LOCK0 + Math.floor(rng() * P.colors); }
      for (var gr = 0; gr < P.grates; gr++) { var pg = take(); grid[pg[0]][pg[1]] = GRATE0 + Math.floor(rng() * 4); }

      grid = settle(grid);                      // pieces start at rest
      if (findGroups(grid).length) continue;    // no free merges at spawn

      var found = search(grid, P.budget);
      if (found.best < MIN_BEST) continue;      // too barren — re-salt

      var quota = Math.max(MERGE_MIN, Math.ceil(found.best * 0.6));
      var solution = null;
      for (var i = 0; i < found.records.length; i++)
        if (found.records[i].collected >= quota) { solution = found.records[i].path; break; }
      if (!solution) continue;                  // defensive; cannot happen

      return {
        size: SIZE, grid: grid, colors: P.colors,
        quota: quota, budget: P.budget,
        seed: seed >>> 0, levelIndex: levelIndex, salt: salt,
        best: found.best, solution: solution
      };
    }
    throw new Error("generateLevel: no solvable layout in " + MAX_SALT + " salts (seed=" + seed + ", level=" + levelIndex + ")");
  }

  // ------------------------------------------------ level packs (slice 5) --

  // How hard is a generated level, honestly? Two axes the solver already
  // measured at generation time: PAR (the BFS-shortest winning line —
  // longer = more forced reading) and SLACK (best - quota: how many
  // collectible gems the cavern offers beyond what it demands — fewer
  // spare gems = less room to waste a pop). Pure integer scalar, higher =
  // harder: par dominates and slack breaks ties DOWNWARD, so sorting by
  // score descending is exactly (par desc, slack asc) — the concept doc's
  // "long solutions, low slack".
  function difficulty(level) {
    var p = par(level);
    var slack = level.best - level.quota;
    var score = p * 1000 + Math.max(0, 999 - slack);
    var label = p >= 5 ? "CRUEL" : p === 4 ? "STIFF" : p === 3 ? "TRICKY" : "MILD";
    return { par: p, slack: slack, score: score, label: label };
  }

  // Deterministic curation: rate EVERY seed in the def's scan window and
  // keep the `take` hardest (score desc, seed asc tiebreak — total order,
  // no RNG beyond the generator's own). Pure function of the def, so
  // re-running it MUST reproduce the shipped PACKS pin below; the smoke
  // asserts exactly that, which is what keeps the curated data honest —
  // never hand-edited, always the solver's own verdict.
  function curatePack(def) {
    var rated = [];
    for (var s = def.scanFrom; s < def.scanFrom + def.scanCount; s++) {
      var d = difficulty(generateLevel(s, def.levelIndex));
      rated.push({ seed: s >>> 0, par: d.par, slack: d.slack, score: d.score });
    }
    rated.sort(function (a, b) { return b.score - a.score || a.seed - b.seed; });
    return {
      id: def.id, name: def.name, levelIndex: def.levelIndex,
      entries: rated.slice(0, def.take)
    };
  }

  // The shipped curation recipes. GRANITE GAUNTLET is base-rules hardcore
  // (level 0: no slice-4 cells); DEEP CUTS curates the deep caverns
  // (level 4: ice + locked gems + grates live). Both scan the same 32-seed
  // window so the two packs are the same population rated at two depths.
  var PACK_DEFS = [
    { id: "granite-gauntlet", name: "GRANITE GAUNTLET", levelIndex: 0, scanFrom: 1, scanCount: 32, take: 6 },
    { id: "deep-cuts",        name: "DEEP CUTS",        levelIndex: 4, scanFrom: 1, scanCount: 32, take: 6 }
  ];

  // ...and their PINNED result, so the page never re-runs 64 BFS curations
  // at load time. Computed BY curatePack itself and pinned verbatim; the
  // engine smoke re-derives both packs and asserts JSON byte-equality.
  var PACKS = [
    { id: "granite-gauntlet", name: "GRANITE GAUNTLET", levelIndex: 0, entries: [
      { seed: 1,  par: 7, slack: 4, score: 7995 },
      { seed: 3,  par: 6, slack: 4, score: 6995 },
      { seed: 8,  par: 6, slack: 4, score: 6995 },
      { seed: 10, par: 5, slack: 4, score: 5995 },
      { seed: 29, par: 5, slack: 4, score: 5995 },
      { seed: 32, par: 5, slack: 5, score: 5994 }
    ] },
    { id: "deep-cuts", name: "DEEP CUTS", levelIndex: 4, entries: [
      { seed: 12, par: 8, slack: 5, score: 8994 },
      { seed: 7,  par: 8, slack: 7, score: 8992 },
      { seed: 18, par: 7, slack: 6, score: 7993 },
      { seed: 20, par: 7, slack: 6, score: 7993 },
      { seed: 4,  par: 7, slack: 7, score: 7992 },
      { seed: 21, par: 7, slack: 7, score: 7992 }
    ] }
  ];

  function packById(id) {
    for (var i = 0; i < PACKS.length; i++) if (PACKS[i].id === id) return PACKS[i];
    return null;
  }

  // -------------------------------------------------------------- state --

  function newGame(seed, levelIndex) {
    var level = generateLevel(seed, levelIndex || 0);
    return {
      level: level,
      grid: cloneGrid(level.grid),
      size: level.size,
      quota: level.quota,
      budget: level.budget,
      used: 0,
      collected: 0,
      status: "playing",           // 'playing' | 'won' | 'lost'
      events: [],                  // collect events of the LAST rotation
      seed: level.seed,
      levelIndex: level.levelIndex
    };
  }

  // The one move in the game. Pure: returns a NEW state, input untouched.
  // dir: 'cw' | 'ccw'. No-op unless status === 'playing'.
  function rotate(state, dir) {
    if (state.status !== "playing") return state;
    if (dir !== "cw" && dir !== "ccw") throw new Error("rotate: dir must be 'cw'|'ccw'");
    var res = resolve(rotateGrid(state.grid, dir));
    var collected = state.collected + res.collected;
    var used = state.used + 1;
    var status = collected >= state.quota ? "won" : (used >= state.budget ? "lost" : "playing");
    return {
      level: state.level, grid: res.grid, size: state.size,
      quota: state.quota, budget: state.budget,
      used: used, collected: collected, status: status,
      events: res.events, seed: state.seed, levelIndex: state.levelIndex
    };
  }

  // Replay a rotation line like "LRRL" (L=ccw, R=cw). Pure.
  function replay(state, line) {
    for (var i = 0; i < line.length; i++)
      state = rotate(state, line[i] === "R" ? "cw" : "ccw");
    return state;
  }

  // ------------------------------------------- shareable line (arc 2, cut 1) --

  // Determinism is TOTAL, so a cleared (or attempted) cavern is fully described
  // by three things: the SEED, the levelIndex, and the ROTATION LINE the player
  // took (a string of 'L'/'R'). That triple replays the identical game anywhere.
  // These pure helpers turn the triple into a shareable query string and back,
  // and re-drive it through resolveTrace for the full scored, animated playback —
  // the social half of the daily-seed hook: today players share the CAVERN
  // (?seed=N), now they share their LINE too. Only the player's OWN line is ever
  // encoded; the solver's stays hidden, so par bragging rights survive. No DOM,
  // no clock, no URL object — just strings. All ADDITIVE: nothing above is edited.

  var LINE_RE = /^[LR]*$/;
  var MAX_LINE = 64;   // generous cap (budget is 10); guards a hostile URL

  // Is `s` a legal rotation line? (a string of only 'L'/'R', within the cap)
  function isReplayLine(s) {
    return typeof s === "string" && s.length <= MAX_LINE && LINE_RE.test(s);
  }

  // Normalize a raw line: uppercase, drop every non-L/R char, cap the length.
  // Lets a hand-typed or arrow-glyph share degrade to its legal core instead of
  // throwing. Pure.
  function normalizeLine(s) {
    if (typeof s !== "string") return "";
    var out = "";
    for (var i = 0; i < s.length && out.length < MAX_LINE; i++) {
      var ch = s.charAt(i).toUpperCase();
      if (ch === "L" || ch === "R") out += ch;
    }
    return out;
  }

  // Encode a share as the canonical query fragment `seed=N&level=L&replay=RRLR`
  // (`level` omitted when 0 — the common daily case — so the URL stays short).
  // Pure; the leading '?' is the caller's business. THROWS on an illegal line so
  // a share is never silently corrupted at creation.
  function encodeShare(share) {
    var seed = (share && share.seed) >>> 0;
    var level = Math.max(0, (share && share.levelIndex) | 0);
    var line = share && share.line != null ? String(share.line) : "";
    if (!isReplayLine(line)) throw new Error("encodeShare: line must match /^[LR]*$/ within " + MAX_LINE + ", got: " + line);
    var parts = ["seed=" + seed];
    if (level > 0) parts.push("level=" + level);
    parts.push("replay=" + line);
    return parts.join("&");
  }

  function safeDecodeURIComponent(s) {
    try { return decodeURIComponent(s); } catch (e) { return null; }
  }

  // Decode a share from a query string ('?a=b&c=d' or 'a=b&c=d'), a
  // URLSearchParams-like object (has .get()), or a plain {seed,level,replay} bag.
  // Returns { seed, levelIndex, line } with the line NORMALIZED to its legal
  // core, or null when there is no usable seed+replay pair. Pure — never touches
  // the URL/DOM; a malformed %-escape degrades to null, never throws.
  function decodeShare(src) {
    var get;
    if (src && typeof src.get === "function") {            // URLSearchParams-like
      get = function (k) { return src.get(k); };
    } else if (typeof src === "string") {
      var q = src.charAt(0) === "?" ? src.slice(1) : src;
      var map = {}, bad = false;
      q.split("&").forEach(function (kv) {
        if (!kv) return;
        var eq = kv.indexOf("=");
        var rawK = eq < 0 ? kv : kv.slice(0, eq);
        var rawV = eq < 0 ? "" : kv.slice(eq + 1);
        var k = safeDecodeURIComponent(rawK), v = safeDecodeURIComponent(rawV);
        if (k === null || v === null) { bad = true; return; }
        map[k] = v;
      });
      if (bad) return null;
      get = function (k) { return Object.prototype.hasOwnProperty.call(map, k) ? map[k] : null; };
    } else if (src && typeof src === "object") {
      get = function (k) { return src[k] != null ? String(src[k]) : null; };
    } else {
      return null;
    }
    var rawSeed = get("seed"), rawReplay = get("replay");
    if (rawSeed == null || rawReplay == null) return null;
    var seed = parseInt(rawSeed, 10);
    if (!isFinite(seed)) return null;
    var level = parseInt(get("level") || "0", 10);
    if (!isFinite(level) || level < 0) level = 0;
    return { seed: seed >>> 0, levelIndex: level, line: normalizeLine(rawReplay) };
  }

  // Replay a shared line as a SPECTATED run: generate the (seed, levelIndex)
  // cavern, then apply the line one rotation at a time, capturing for each the
  // resolveTrace phases (what the shell tweens) and the post-rotation state.
  // Stops early if the game reaches a terminal state before the line ends (a won
  // cavern freezes; extra rotations are ignored, exactly like the live shell).
  // Pure — reuses newGame/rotate/resolveTrace, so the reconstructed run is
  // byte-identical to the sharer's own play. Returns:
  //   { start, steps:[{ dir, from, trace, state }], final, consumed, clean }
  // `clean` is true when the input line was already legal (nothing normalized away).
  function spectate(seed, levelIndex, line) {
    var clean = isReplayLine(typeof line === "string" ? line : "");
    var norm = normalizeLine(line);
    var start = newGame(seed >>> 0, levelIndex | 0);
    var state = start, steps = [];
    for (var i = 0; i < norm.length; i++) {
      if (state.status !== "playing") break;
      var dir = norm.charAt(i) === "R" ? "cw" : "ccw";
      var from = state.grid;
      var trace = resolveTrace(rotateGrid(from, dir));
      state = rotate(state, dir);
      steps.push({ dir: dir, from: from, trace: trace, state: state });
    }
    return { start: start, steps: steps, final: state, consumed: steps.length, clean: clean };
  }

  // ------------------------------------------- solver hints (arc 2, cut 2) --

  // The solver already stores each cavern's shortest winning line (level.solution);
  // that line "doubles as a free hint system". hintFrom() surfaces the ONE next
  // rotation of a shortest winning continuation FROM THE CURRENT board — not the
  // whole solution, a single nudge. It re-runs the same BFS `search` from
  // state.grid against the REMAINING budget for the REMAINING quota, so it stays
  // honest after the player has wandered off the solver's pristine line (a detour
  // or an undo): the hint always points along a shortest win from where you
  // actually are, never a stale parrot of the stored line. Spend-gated — a hint is
  // meant to cost, so hintedGrade() folds the hint count into `used`, dinging the
  // card exactly like the same number of over-par turns. Never reveals a hint for
  // a terminal (won/lost) board. Pure — reuses search/resolve, no DOM, no clock,
  // no generator change. All ADDITIVE: nothing above is edited.

  // The next rotation ('L'=ccw / 'R'=cw) of a shortest winning line from THIS
  // board, or null when the board is terminal OR no line still reaches quota from
  // here within the remaining budget. Deterministic: it selects the first shortest
  // quota-meeting continuation in the same BFS order `solve` uses, so from a
  // pristine board it agrees with level.solution[0].
  function hintFrom(state) {
    if (!state || state.status !== "playing") return null;
    var need = (state.quota | 0) - (state.collected | 0);
    if (need <= 0) return null;                         // quota already met
    var left = (state.budget | 0) - (state.used | 0);
    if (left <= 0) return null;                         // no turns remain
    var found = search(state.grid, left);
    for (var i = 0; i < found.records.length; i++)
      if (found.records[i].collected >= need) return found.records[i].path.charAt(0);
    return null;                                        // unwinnable from here
  }

  // Spend-gated grade: each hint taken counts like one extra (over-par) turn, so
  // leaning on the solver dings your card. hints<=0 reproduces plain grade(). Pure.
  function hintedGrade(used, parTurns, hints) {
    return grade((used | 0) + Math.max(0, hints | 0), parTurns);
  }

  // ---- undo×par deception curation (arc 2, cut 3) -----------------------------
  // Rate a cleared level's *deceptiveness* from the (undos, overshoot) pair.
  //   overshoot = turns used beyond par  -> honest, visible forward effort
  //   undos     = in-place backtracking  -> hidden effort; what makes a level deceptive
  // A win at par that ate many undos is HARD-deceptive; par+2 with no undos is
  // medium-honest. Pure integers, no I/O -- the shell persists the pair in guarded
  // localStorage and renders deceptionLabel().
  function deception(undos, used, parTurns) {
    var u = undos > 0 ? (undos | 0) : 0;
    var overshoot = used - parTurns;
    if (overshoot < 0) overshoot = 0;
    var d = u * 2 - overshoot;
    return d > 0 ? d : 0;
  }

  function deceptionLabel(undos, used, parTurns) {
    var u = undos > 0 ? (undos | 0) : 0;
    var overshoot = used - parTurns;
    if (overshoot < 0) overshoot = 0;
    var effort = u + overshoot;
    var tier = effort === 0 ? "clean" : (effort <= 3 ? "medium" : "HARD");
    var d = deception(undos, used, parTurns);
    var honesty = d === 0 ? "honest" : (d <= 3 ? "tricky" : "deceptive");
    return tier + "-" + honesty;
  }

  // ---- mechanic fingerprint: per-class par deltas (arc 2, cut 4) --------------
  // Which slice-4 cell CLASS actually carries a cavern? Re-run the solver's own BFS
  // on the grid with ONE class NEUTRALIZED and read how par moves:
  //   ice   -> slip disabled  (ICE becomes STONE: same dead-weight fall, no slide)
  //   lock  -> pre-freed      (each locked gem becomes a normal gem of its color)
  //   grate -> all-porous     (grate cells removed: no one-way barrier remains)
  // Neutralization is a PURE grid transform fed to `search` as a parallel input --
  // zero edits to settle/resolve/rotate, so every prior pin stays byte-identical.
  // If neutralizing a class the cavern actually places moves par (delta != 0) OR
  // kills the level (no winning line within budget -> dead), that mechanic is
  // load-bearing: an honest per-seed tag ("this daily NEEDS the grate"). A class
  // the cavern never places is a no-op (delta 0, not load-bearing). All ADDITIVE.

  var FP_CLASSES = ["ice", "lock", "grate"];

  function classAt(v, cls) {
    return cls === "ice"   ? v === ICE
         : cls === "lock"  ? isLocked(v)
         : cls === "grate" ? isGrate(v)
         : false;
  }

  // Grid with every cell of ONE class replaced by its neutral stand-in. Pure:
  // returns a NEW grid, input untouched; cells of any other class pass through.
  // On content with none of `cls` the result is byte-identical to the input.
  function neutralizeClass(g, cls) {
    var n = g.length, out = cloneGrid(g);
    for (var r = 0; r < n; r++)
      for (var c = 0; c < n; c++) {
        var v = g[r][c];
        if (cls === "ice"   && v === ICE)   out[r][c] = STONE;
        else if (cls === "lock"  && isLocked(v)) out[r][c] = GEM0 + (v - LOCK0);
        else if (cls === "grate" && isGrate(v)) out[r][c] = EMPTY;
      }
    return out;
  }

  function hasClass(g, cls) {
    for (var r = 0; r < g.length; r++)
      for (var c = 0; c < g.length; c++)
        if (classAt(g[r][c], cls)) return true;
    return false;
  }

  // Shortest winning depth for `quota` on `grid` within `budget`, or null when no
  // rotation line up to `budget` deep reaches quota. `search` pushes records
  // shortest-first, so the first quota-meeting record's depth IS the minimum. Pure.
  function shortestWinDepth(grid, budget, quota) {
    var found = search(grid, budget);
    for (var i = 0; i < found.records.length; i++)
      if (found.records[i].collected >= quota) return found.records[i].depth;
    return null;
  }

  // Fingerprint a level: base par plus, per slice-4 class, the neutralized par and
  // its signed delta from base. `dead` marks a class the level cannot win without;
  // a class is `loadBearing` when it is PRESENT and neutralizing it moves par or
  // kills the level. Deterministic — a pure function of the level's grid/budget/
  // quota. The base equals par(level) for a generated level (both the BFS shortest
  // line). Absent classes report present:false, delta:0, not load-bearing.
  function fingerprint(level) {
    var base = shortestWinDepth(level.grid, level.budget, level.quota);
    var out = { base: base };
    for (var i = 0; i < FP_CLASSES.length; i++) {
      var cls = FP_CLASSES[i];
      if (!hasClass(level.grid, cls)) {
        out[cls] = { present: false, par: base, delta: 0, dead: false, loadBearing: false };
        continue;
      }
      var np = shortestWinDepth(neutralizeClass(level.grid, cls), level.budget, level.quota);
      var dead = np === null;
      var delta = dead ? null : np - base;
      out[cls] = { present: true, par: np, delta: delta, dead: dead,
                   loadBearing: dead || delta !== 0 };
    }
    return out;
  }

  // How load-bearing a class is, as a sortable scalar: a level that DIES without it
  // outranks any finite par shift; otherwise the size of the par move. Pure.
  function fpLoad(entry) {
    return entry.dead ? Infinity : Math.abs(entry.delta || 0);
  }

  // Honest per-seed tag: the load-bearing mechanics, heaviest carrier first
  // ("NEEDS grate", "NEEDS lock+ice"), or "no-mechanic" when neutralizing every
  // class leaves par unmoved (a base cavern, or a deep one its mechanics do not
  // gate). Deterministic: equal loads break ties in FP_CLASSES order. Pure.
  function fingerprintTag(level) {
    var fp = fingerprint(level);
    var carried = [];
    for (var i = 0; i < FP_CLASSES.length; i++)
      if (fp[FP_CLASSES[i]].loadBearing) carried.push(FP_CLASSES[i]);
    if (!carried.length) return "no-mechanic";
    carried.sort(function (a, b) {
      var la = fpLoad(fp[a]), lb = fpLoad(fp[b]);
      if (lb !== la) return lb - la;
      return FP_CLASSES.indexOf(a) - FP_CLASSES.indexOf(b);
    });
    return "NEEDS " + carried.join("+");
  }

  return {
    VERSION: VERSION, SIZE: SIZE,
    EMPTY: EMPTY, WALL: WALL, STONE: STONE, GEM0: GEM0, MERGE_MIN: MERGE_MIN,
    ICE: ICE, LOCK0: LOCK0, GRATE0: GRATE0,
    isGem: isGem, isLocked: isLocked, isGrate: isGrate,
    mulberry32: mulberry32, mixSeed: mixSeed, dailySeed: dailySeed,
    emptyGrid: emptyGrid, cloneGrid: cloneGrid, gridString: gridString,
    rotateGrid: rotateGrid, settle: settle, findGroups: findGroups, resolve: resolve,
    settleMoves: settleMoves, resolveTrace: resolveTrace,
    search: search, solve: solve, par: par, grade: grade,
    difficulty: difficulty, curatePack: curatePack,
    PACK_DEFS: PACK_DEFS, PACKS: PACKS, packById: packById,
    paramsFor: paramsFor, generateLevel: generateLevel,
    newGame: newGame, rotate: rotate, replay: replay,
    isReplayLine: isReplayLine, normalizeLine: normalizeLine,
    encodeShare: encodeShare, decodeShare: decodeShare, spectate: spectate,
    hintFrom: hintFrom, hintedGrade: hintedGrade,
    deception: deception, deceptionLabel: deceptionLabel,
    neutralizeClass: neutralizeClass, fingerprint: fingerprint, fingerprintTag: fingerprintTag
  };
});
