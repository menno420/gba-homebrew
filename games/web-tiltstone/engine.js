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

  var VERSION = "1.1.0";
  var SIZE = 8;          // grid is SIZE x SIZE (square — rotation-safe)
  var EMPTY = 0, WALL = 1, STONE = 2, GEM0 = 3;
  var MERGE_MIN = 3;     // orthogonal group size that collects
  var MAX_SALT = 64;     // generation re-tries before giving up (never hit in practice)
  var MIN_BEST = 6;      // a layout must offer at least this many collectible gems

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

  var CHARS = ".#oABCDEFGH"; // index by cell code: gems are A.., stone o, wall #
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

  // Rotate the WHOLE cavern (walls included) 90 degrees.
  function rotateGrid(g, dir) {
    var n = g.length, out = emptyGrid(n);
    for (var r = 0; r < n; r++)
      for (var c = 0; c < n; c++)
        out[r][c] = dir === "cw" ? g[n - 1 - c][r] : g[c][n - 1 - r];
    return out;
  }

  // Gravity: movable cells (stone + gems) fall straight down within their
  // column until the floor, a wall, or another piece stops them. Walls stay
  // put. Returns a NEW grid.
  function settle(g) {
    var n = g.length, out = emptyGrid(n);
    for (var c = 0; c < n; c++) {
      for (var r = 0; r < n; r++) if (g[r][c] === WALL) out[r][c] = WALL;
      var write = n - 1;
      for (var r2 = n - 1; r2 >= 0; r2--) {
        var v = g[r2][c];
        if (v === WALL) { write = r2 - 1; continue; }
        if (v >= STONE) {
          while (write >= 0 && out[write][c] === WALL) write--;
          out[write][c] = v;
          write--;
        }
      }
    }
    return out;
  }

  // Find all orthogonal same-color gem groups of size >= MERGE_MIN.
  // Returns [{color, cells:[[r,c],...]}, ...] in deterministic scan order.
  function findGroups(g) {
    var n = g.length, seen = emptyGrid(n), groups = [];
    for (var r = 0; r < n; r++) {
      for (var c = 0; c < n; c++) {
        var v = g[r][c];
        if (v < GEM0 || seen[r][c]) continue;
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

  // Settle, then repeatedly collect groups and re-settle (cascade chains).
  // Pure: takes a grid, returns { grid, collected, events }.
  function resolve(g) {
    var grid = settle(g), collected = 0, events = [], chain = 0;
    for (;;) {
      var groups = findGroups(grid);
      if (!groups.length) break;
      chain++;
      for (var i = 0; i < groups.length; i++) {
        var grp = groups[i];
        collected += grp.cells.length;
        events.push({ type: "collect", color: grp.color, size: grp.cells.length, chain: chain });
        for (var j = 0; j < grp.cells.length; j++) grid[grp.cells[j][0]][grp.cells[j][1]] = EMPTY;
      }
      grid = settle(grid);
    }
    return { grid: grid, collected: collected, events: events };
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

  // Level parameters ramp gently with levelIndex (colors 3 -> 4 at level 3).
  function paramsFor(levelIndex) {
    return {
      colors: levelIndex >= 3 ? 4 : 3,
      gemsPerColor: 5 + (levelIndex >= 2 ? 1 : 0),
      stones: 4,
      walls: 6 + (levelIndex % 3),
      budget: 10
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

  return {
    VERSION: VERSION, SIZE: SIZE,
    EMPTY: EMPTY, WALL: WALL, STONE: STONE, GEM0: GEM0, MERGE_MIN: MERGE_MIN,
    mulberry32: mulberry32, mixSeed: mixSeed, dailySeed: dailySeed,
    emptyGrid: emptyGrid, cloneGrid: cloneGrid, gridString: gridString,
    rotateGrid: rotateGrid, settle: settle, findGroups: findGroups, resolve: resolve,
    search: search, solve: solve, par: par, grade: grade,
    paramsFor: paramsFor, generateLevel: generateLevel,
    newGame: newGame, rotate: rotate, replay: replay
  };
});
