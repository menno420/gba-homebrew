/* Tiltstone juice — animation timing + synth audio recipes (slice 3).
 *
 * Split exactly like engine.js: everything that DECIDES is pure and
 * headless (the timeline builder and the cue table are plain functions of
 * their arguments — no DOM, no clock, no randomness), so the same file is
 * a proof surface in Node/CI. Only makeAudio() touches the browser
 * (WebAudio), and it is guarded — no AudioContext means silent no-ops, and
 * audio is NEVER load-bearing: every cue is appended to an honest log
 * whether or not it is audible (the "which sound is asked for WHEN"
 * surface, provable headlessly; the mix itself is playtest material).
 *
 * 100% synthesized: oscillator sweeps only. No sampled or licensed audio,
 * ever (repo-wide rule).
 *
 * Dual-mode loading:
 *   - browser: <script src="juice.js"> -> window.TiltstoneJuice
 *   - Node:    const J = require("./juice.js")
 */
"use strict";

(function (root, factory) {
  var api = factory();
  if (typeof module !== "undefined" && module.exports) module.exports = api;
  else root.TiltstoneJuice = api;
})(typeof self !== "undefined" ? self : this, function () {

  var VERSION = "1.0.0";

  // ------------------------------------------------------------- timing --

  // One place for every duration (ms). The board rotation sweep is prepended
  // by the shell; fall time scales with the LONGEST drop in the step so a
  // one-row nudge is snappy and a full-column plunge reads as a plunge.
  var TIMING = {
    rotateMs: 150,        // whole-board 90-degree sweep
    fallPerCellMs: 40,    // per row of the longest drop in the step
    fallMinMs: 70,
    fallMaxMs: 260,
    popMs: 160,           // one collect group flashing out
    chainGapMs: 70        // beat between cascade chains
  };

  // Pure: engine resolveTrace phases -> absolute-ms replay schedule.
  // Returns { steps, total }. Each step is the phase plus { t0, t1, cues }:
  //   fall    -> cue "land" when it ends (skipped when nothing moved —
  //              empty falls produce NO step at all)
  //   collect -> cue "collect" (chain-indexed) when it starts
  // Steps are sequential and non-overlapping; a chainGap beat is inserted
  // when the chain number rises. Deterministic: same phases, same schedule.
  function timeline(phases, timing) {
    var T = timing || TIMING;
    var t = 0, steps = [], lastChain = 0;
    for (var i = 0; i < phases.length; i++) {
      var ph = phases[i];
      if (ph.type === "fall") {
        if (!ph.moves.length) continue;
        var maxDrop = 0;
        for (var m = 0; m < ph.moves.length; m++) {
          var d = ph.moves[m].to[0] - ph.moves[m].from[0];
          if (d > maxDrop) maxDrop = d;
        }
        var dur = Math.max(T.fallMinMs, Math.min(T.fallMaxMs, maxDrop * T.fallPerCellMs));
        steps.push({ phase: ph, t0: t, t1: t + dur, cues: [{ name: "land", at: "end" }] });
        t += dur;
      } else { // collect
        if (ph.chain !== lastChain) { t += lastChain > 0 ? T.chainGapMs : 0; lastChain = ph.chain; }
        steps.push({ phase: ph, t0: t, t1: t + T.popMs, cues: [{ name: "collect", chain: ph.chain, at: "start" }] });
        t += T.popMs;
      }
    }
    return { steps: steps, total: t };
  }

  // ---------------------------------------------------------- cue table --

  // Every sound in the game, as data: a single oscillator sweep per cue
  // (wave, f0 -> f1 Hz over dur seconds, peak gain). Chain-indexed collects
  // pitch UP a major-third-ish ratio per cascade step, capped, so a chain
  // SOUNDS like a chain.
  var CUES = {
    rotate:  { wave: "triangle", f0: 220, f1: 150,  dur: 0.09, gain: 0.12 },
    land:    { wave: "square",   f0: 100, f1: 65,   dur: 0.07, gain: 0.10 },
    collect: { wave: "sine",     f0: 523, f1: 784,  dur: 0.16, gain: 0.16 },
    win:     { wave: "triangle", f0: 523, f1: 1046, dur: 0.35, gain: 0.18 },
    lose:    { wave: "square",   f0: 150, f1: 55,   dur: 0.40, gain: 0.15 },
    undo:    { wave: "sine",     f0: 330, f1: 262,  dur: 0.08, gain: 0.10 }
  };
  var CHAIN_RATIO = 1.25, CHAIN_CAP = 4;

  // Pure: cue name (+ chain for collects) -> one concrete synth recipe,
  // or null for an unknown name. `key` is the honest-log token.
  function cueFor(name, chain) {
    var base = CUES[name];
    if (!base) return null;
    var f0 = base.f0, f1 = base.f1, key = name;
    if (name === "collect" && (chain | 0) > 1) {
      var k = Math.min((chain | 0) - 1, CHAIN_CAP);
      f0 = Math.round(f0 * Math.pow(CHAIN_RATIO, k));
      f1 = Math.round(f1 * Math.pow(CHAIN_RATIO, k));
      key = name + "@x" + (chain | 0);
    }
    return { key: key, wave: base.wave, f0: f0, f1: f1, dur: base.dur, gain: base.gain };
  }

  // ------------------------------------------------- browser audio player --

  // Guarded WebAudio player. play() ALWAYS appends the cue key to the log
  // (muted or silent environments included — the log is the proof surface);
  // sound itself is best-effort and wrapped, because audio must never break
  // the game. The AudioContext is created lazily on the first play, which
  // in practice is always inside a user-gesture handler (autoplay policy).
  function makeAudio() {
    var ctx = null, muted = false, log = [];
    function ensure() {
      if (ctx === null && typeof window !== "undefined") {
        var AC = window.AudioContext || window.webkitAudioContext;
        ctx = AC ? new AC() : false; // false = probed, unavailable
      }
      if (ctx && ctx.state === "suspended") ctx.resume();
      return ctx || null;
    }
    return {
      play: function (name, chain) {
        var cue = cueFor(name, chain);
        if (!cue) return null;
        log.push(cue.key);
        if (muted) return cue;
        try {
          var ac = ensure();
          if (!ac) return cue;
          var o = ac.createOscillator(), g = ac.createGain(), t0 = ac.currentTime;
          o.type = cue.wave;
          o.frequency.setValueAtTime(cue.f0, t0);
          o.frequency.exponentialRampToValueAtTime(Math.max(1, cue.f1), t0 + cue.dur);
          g.gain.setValueAtTime(cue.gain, t0);
          g.gain.exponentialRampToValueAtTime(0.0001, t0 + cue.dur);
          o.connect(g); g.connect(ac.destination);
          o.start(t0); o.stop(t0 + cue.dur + 0.02);
        } catch (e) { /* audio is never load-bearing */ }
        return cue;
      },
      setMuted: function (m) { muted = !!m; },
      isMuted: function () { return muted; },
      getLog: function () { return log.slice(); }
    };
  }

  return {
    VERSION: VERSION,
    TIMING: TIMING, CUES: CUES,
    CHAIN_RATIO: CHAIN_RATIO, CHAIN_CAP: CHAIN_CAP,
    timeline: timeline, cueFor: cueFor, makeAudio: makeAudio
  };
});
