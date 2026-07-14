#!/usr/bin/env python3
"""Deterministic original sound effects for DEEPCAST (growth cut 1 —
the concept's named audio line: "reel clicks that speed up with
tension — tension readable with eyes closed").

The Lumen Drift / Shoal / Clockwork Courier pattern
(games/shoal/audio/generate_audio.py): a stdlib-only generator whose
output is byte-exact on every run (pure integer/float math, an explicit
LCG for noise — no `random` module, no timestamps), so the committed
WAVs can be regenerated and diffed. Butano's maxmod pipeline (mmutil)
converts the WAVs into the ROM soundbank at build time; the game plays
them as bn::sound_items::dc_*.

All four sounds are synthesized from scratch (sines, an LCG noise
source, exponential envelopes) — original audio, no samples, ever.

The lake's palette: one DRY thing (the reel — a hard ratchet tick, the
only percussive voice, because the click train IS the tension gauge)
against wet, round everything-else (bite plunge, catch resolve, snap
twang-into-wash). The click is deliberately short (~28ms) so the game
can retrigger it at the highest click rate (every 4 frames ≈ 67ms)
without voices piling up.

Format: mono 16-bit PCM WAV @ 16384 Hz (matches maxmod's GBA mixing
family). Run from this directory:  python3 generate_audio.py
"""

import math
import struct

RATE = 16384


def lcg(seed):
    """Deterministic 32-bit LCG noise stream in [-1, 1) (Numerical Recipes)."""
    state = seed
    while True:
        state = (state * 1664525 + 1013904223) & 0xFFFFFFFF
        yield (state / 2147483648.0) - 1.0


def write_wav(name, samples):
    data = b''.join(struct.pack('<h', max(-32767, min(32767, int(s * 32767))))
                    for s in samples)
    header = struct.pack('<4sI4s4sIHHIIHH4sI', b'RIFF', 36 + len(data),
                         b'WAVE', b'fmt ', 16, 1, 1, RATE, RATE * 2, 2, 16,
                         b'data', len(data))
    with open(name, 'wb') as handle:
        handle.write(header + data)
    print(f'{name}: {len(samples)} samples ({len(samples) / RATE:.2f}s)')


def env(i, n, attack=0.01, curve=5.0):
    """Attack-then-exponential-decay envelope over n samples."""
    t = i / n
    a = min(1.0, t / attack) if attack > 0 else 1.0
    return a * math.exp(-curve * t)


def dc_click():
    """The reel pawl: one dry ratchet tick — a hard noise transient over
    a bright metallic partial pair, dead in ~28ms. The game retriggers
    this faster (and pitched higher) as tension climbs; the sound itself
    stays constant so the RATE carries the information."""
    n = int(RATE * 0.028)
    noise = lcg(0xDEE9C11C)
    out = []
    for i in range(n):
        t = i / n
        s = 0.55 * next(noise) * math.exp(-18.0 * t)      # the pawl snap
        s += 0.40 * math.sin(2 * math.pi * 2093.0 * i / RATE)   # C7 ring
        s += 0.18 * math.sin(2 * math.pi * 3135.96 * i / RATE)  # G7 sheen
        out.append(s * env(i, n, attack=0.002, curve=11.0))
    return out


def dc_bite():
    """The strike: a wet downward plunge — a sine dropping an octave
    (E5 -> E4) into a soft lowpassed bubble of noise, like a float
    yanked under."""
    n = int(RATE * 0.14)
    noise = lcg(0xB17EB17E)
    low = 0.0
    out = []
    for i in range(n):
        t = i / n
        f = 659.26 * math.exp(-0.693 * t)   # one octave down over the hit
        raw = next(noise)
        low = low + 0.10 * (raw - low)      # one-pole lowpass bubble
        s = 0.72 * math.sin(2 * math.pi * f * i / RATE)
        s += 0.22 * low * math.exp(-4.0 * t)
        out.append(s * env(i, n, attack=0.004, curve=6.5))
    return out


def dc_catch():
    """Landed: a small warm resolve rising a fourth (A4 -> D5) over a
    low water swell — score in the boat."""
    n = int(RATE * 0.34)
    half = n // 2
    out = []
    for i in range(n):
        f = 440.0 if i < half else 587.33
        local = i if i < half else i - half
        span = half if i < half else n - half
        s = 0.58 * math.sin(2 * math.pi * f * i / RATE)
        s += 0.22 * math.sin(2 * math.pi * f * 0.5 * i / RATE)
        s += 0.10 * math.sin(2 * math.pi * 110.0 * i / RATE)
        out.append(s * env(local, span, attack=0.02, curve=4.0))
    return out


def dc_snap():
    """The line breaks: a high twang whipping down fast (E6 falling) with
    a noise crack, sinking into a short cold wash — the lure is gone."""
    n = int(RATE * 0.30)
    noise = lcg(0x5A45BAD)
    low = 0.0
    out = []
    for i in range(n):
        t = i / n
        f = 1318.51 * math.exp(-3.0 * t)    # the whip down
        raw = next(noise)
        low = low + 0.06 * (raw - low)
        s = 0.62 * math.sin(2 * math.pi * f * i / RATE)
        s += 0.30 * raw * math.exp(-30.0 * t)          # the crack
        s += 0.22 * low * min(1.0, t * 2.5)            # the cold wash
        out.append(s * env(i, n, attack=0.002, curve=4.5))
    return out


if __name__ == '__main__':
    write_wav('dc_click.wav', dc_click())
    write_wav('dc_bite.wav', dc_bite())
    write_wav('dc_catch.wav', dc_catch())
    write_wav('dc_snap.wav', dc_snap())
