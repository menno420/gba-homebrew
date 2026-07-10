#!/usr/bin/env python3
"""Deterministic original sound effects for LUMEN DRIFT (polish pass 2).

Same pattern as graphics/generate_assets.py: a stdlib-only generator whose
output is byte-exact on every run (pure integer/float math, an explicit LCG
for noise — no `random` module, no timestamps), so the committed WAVs can be
regenerated and diffed. Butano's maxmod pipeline (mmutil) converts the WAVs
into the ROM soundbank at build time; the game plays them as
bn::sound_items::ld_*.

All four sounds are synthesized from scratch (sines, a linear-congruential
noise source and exponential envelopes) — original audio, no samples.

Format: mono 16-bit PCM WAV @ 16384 Hz (matches maxmod's GBA mixing family;
mmutil accepts 8/16-bit mono WAV).

Run from this directory:  python3 generate_audio.py
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


def ld_start():
    """Run start: a rising two-note confirm (E5 -> B5), soft square-ish."""
    n = int(RATE * 0.22)
    out = []
    for i in range(n):
        t = i / RATE
        freq = 659.26 if t < 0.10 else 987.77
        local = i if t < 0.10 else i - int(RATE * 0.10)
        span = int(RATE * 0.10) if t < 0.10 else n - int(RATE * 0.10)
        phase = 2 * math.pi * freq * t
        s = math.sin(phase) + 0.3 * math.sin(3 * phase)  # square-ish timbre
        out.append(0.5 * s * env(local, span, attack=0.05, curve=4.0))
    return out


def ld_thrust():
    """Thrust: a soft low rumble — filtered noise over a 55 Hz sine bed."""
    n = int(RATE * 0.35)
    noise = lcg(0xC0FFEE)
    out = []
    filt = 0.0
    for i in range(n):
        t = i / RATE
        filt += 0.08 * (next(noise) - filt)  # one-pole low-pass noise
        bed = 0.45 * math.sin(2 * math.pi * 55 * t)
        out.append(0.55 * (bed + 1.4 * filt) * env(i, n, attack=0.08, curve=3.0))
    return out


def ld_shard():
    """Shard pickup: a bright ascending three-tone glitter (C6-E6-G6)."""
    notes = (1046.50, 1318.51, 1567.98)
    step = int(RATE * 0.07)
    n = step * len(notes) + int(RATE * 0.06)
    out = []
    for i in range(n):
        t = i / RATE
        idx = min(i // step, len(notes) - 1)
        freq = notes[idx]
        phase = 2 * math.pi * freq * t
        s = math.sin(phase) + 0.5 * math.sin(2 * phase)  # octave shimmer
        out.append(0.45 * s * env(i - idx * step, n - idx * step,
                                  attack=0.02, curve=6.0))
    return out


def ld_death():
    """Death: the light is taken — a falling sweep (440 -> 80 Hz), hollow."""
    n = int(RATE * 0.55)
    out = []
    phase = 0.0
    for i in range(n):
        t = i / n
        freq = 440.0 * math.exp(-1.7 * t)  # exponential fall to ~80 Hz
        phase += 2 * math.pi * freq / RATE
        s = math.sin(phase) + 0.35 * math.sin(2 * phase)
        out.append(0.6 * s * env(i, n, attack=0.02, curve=2.5))
    return out


def main():
    write_wav('ld_start.wav', ld_start())
    write_wav('ld_thrust.wav', ld_thrust())
    write_wav('ld_shard.wav', ld_shard())
    write_wav('ld_death.wav', ld_death())


if __name__ == '__main__':
    main()
