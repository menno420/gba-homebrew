#!/usr/bin/env python3
"""Deterministic original sound effects for CLOCKWORK COURIER (growth
rung 4 — the concept's LAST cut).

The Lumen Drift pattern: a stdlib-only generator whose output is
byte-exact on every run (pure integer/float math, an explicit LCG for
noise — no `random` module, no timestamps). Butano's maxmod pipeline
converts the WAVs into the ROM soundbank; the game plays them as
bn::sound_items::cc_*.

All six sounds are synthesized from scratch — original audio, no
samples, ever. The tower's palette: brass and escapements — square-ish
partials, tick transients, short decays. Format: mono 16-bit PCM WAV
@ 16384 Hz. Run from this directory:  python3 generate_audio.py
"""

import math
import struct

RATE = 16384


def lcg(seed):
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
    t = i / n
    a = min(1.0, t / attack) if attack > 0 else 1.0
    return a * math.exp(-curve * t)


def square_ish(f, i):
    """A warm square: fundamental + 3rd + 5th partials."""
    t = i / RATE
    return (0.6 * math.sin(2 * math.pi * f * t)
            + 0.25 * math.sin(2 * math.pi * f * 3 * t)
            + 0.12 * math.sin(2 * math.pi * f * 5 * t))


def cc_start():
    """Run start: a mainspring winding up — a rising buzz, D4 -> A4."""
    n = int(RATE * 0.22)
    out = []
    for i in range(n):
        t = i / n
        f = 293.66 + (440.0 - 293.66) * t
        out.append(0.7 * square_ish(f, i) * env(i, n, attack=0.05, curve=3.0))
    return out


def cc_pickup():
    """Parcel pickup: a bright shelf plink (B5), tiny."""
    n = int(RATE * 0.08)
    out = []
    for i in range(n):
        s = 0.7 * math.sin(2 * math.pi * 987.77 * i / RATE)
        s += 0.2 * math.sin(2 * math.pi * 1975.53 * i / RATE)
        out.append(s * env(i, n, attack=0.004, curve=9.0))
    return out


def cc_rewind():
    """The rewind: time runs backwards — a fast falling sweep with a
    ratchet flutter."""
    n = int(RATE * 0.3)
    out = []
    for i in range(n):
        t = i / n
        f = 880.0 * math.exp(-1.6 * t)
        flutter = 1.0 + 0.25 * math.sin(2 * math.pi * 30.0 * i / RATE)
        out.append(0.65 * math.sin(2 * math.pi * f * flutter * i / RATE)
                   * env(i, n, attack=0.01, curve=2.5))
    return out


def cc_door():
    """The door opens: a low brass clunk-and-slide (G2 thunk into a
    short upward grind)."""
    n = int(RATE * 0.24)
    noise = lcg(0xD00D)
    low = 0.0
    out = []
    for i in range(n):
        t = i / n
        raw = next(noise)
        low = low + 0.08 * (raw - low)
        s = 0.7 * math.sin(2 * math.pi * 98.0 * i / RATE) * math.exp(-6.0 * t)
        s += 0.3 * low * min(1.0, 3.0 * t) * math.exp(-2.5 * t)
        out.append(s * env(i, n, attack=0.003, curve=2.0))
    return out


def cc_deliver():
    """Delivery: the chute swallows the parcel — two-note resolve
    (E5 -> A5) over a soft thump."""
    n = int(RATE * 0.4)
    half = n // 2
    out = []
    for i in range(n):
        f = 659.26 if i < half else 880.0
        local = i if i < half else i - half
        span = half if i < half else n - half
        s = 0.55 * math.sin(2 * math.pi * f * i / RATE)
        s += 0.2 * math.sin(2 * math.pi * 110.0 * i / RATE) * math.exp(-5.0 * i / n)
        out.append(s * env(local, span, attack=0.02, curve=3.5))
    return out


def cc_tick():
    """The chute window opens: one escapement tick — a 4 ms click with
    a metallic ring."""
    n = int(RATE * 0.07)
    out = []
    for i in range(n):
        s = 0.8 * math.sin(2 * math.pi * 2093.0 * i / RATE)
        s += 0.3 * math.sin(2 * math.pi * 3135.96 * i / RATE)
        out.append(s * env(i, n, attack=0.001, curve=14.0))
    return out


if __name__ == '__main__':
    write_wav('cc_start.wav', cc_start())
    write_wav('cc_pickup.wav', cc_pickup())
    write_wav('cc_rewind.wav', cc_rewind())
    write_wav('cc_door.wav', cc_door())
    write_wav('cc_deliver.wav', cc_deliver())
    write_wav('cc_tick.wav', cc_tick())
