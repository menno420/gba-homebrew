#!/usr/bin/env python3
"""Deterministic original sound effects for SHOAL (growth rung 5 — the
concept's LAST cut).

The Lumen Drift pattern (games/lumen-drift/audio/generate_audio.py): a
stdlib-only generator whose output is byte-exact on every run (pure
integer/float math, an explicit LCG for noise — no `random` module, no
timestamps), so the committed WAVs can be regenerated and diffed.
Butano's maxmod pipeline (mmutil) converts the WAVs into the ROM
soundbank at build time; the game plays them as bn::sound_items::sh_*.

All five sounds are synthesized from scratch (sines, an LCG noise
source, exponential envelopes) — original audio, no samples, ever.

The water's palette: everything is round and wet — sine-heavy, low
noise, soft attacks — because the game is herding smoke, not shooting.

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


def sh_start():
    """Run start: a low water bloom — two soft sines rising a fifth
    (A3 -> E4) with a breath of filtered noise, like a current waking."""
    n = int(RATE * 0.28)
    noise = lcg(0x510A17E5)
    low = 0.0
    out = []
    for i in range(n):
        t = i / n
        f = 220.0 + (329.63 - 220.0) * min(1.0, t * 2.2)
        raw = next(noise)
        low = low + 0.04 * (raw - low)          # one-pole lowpass wash
        s = 0.62 * math.sin(2 * math.pi * f * i / RATE)
        s += 0.22 * math.sin(2 * math.pi * f * 0.5 * i / RATE)
        s += 0.18 * low
        out.append(s * env(i, n, attack=0.08, curve=3.5))
    return out


def sh_save():
    """A fish banks: one tiny bright plink (E6), short enough to layer
    when the school streams home."""
    n = int(RATE * 0.09)
    out = []
    for i in range(n):
        s = 0.7 * math.sin(2 * math.pi * 1318.51 * i / RATE)
        s += 0.2 * math.sin(2 * math.pi * 2637.02 * i / RATE)
        out.append(s * env(i, n, attack=0.005, curve=9.0))
    return out


def sh_eaten():
    """A straggler taken: a dull wet thud — pitch-dropping sine into a
    short dark noise snap."""
    n = int(RATE * 0.16)
    noise = lcg(0xBADF00D)
    low = 0.0
    out = []
    for i in range(n):
        t = i / n
        f = 180.0 * math.exp(-2.2 * t)
        raw = next(noise)
        low = low + 0.12 * (raw - low)
        s = 0.75 * math.sin(2 * math.pi * f * i / RATE)
        s += 0.30 * low * math.exp(-6.0 * t)
        out.append(s * env(i, n, attack=0.004, curve=6.0))
    return out


def sh_win():
    """The shoal is home: a warm three-note resolve (A4 - C#5 - E5) over
    a low tide swell."""
    n = int(RATE * 0.55)
    third = n // 3
    out = []
    for i in range(n):
        if i < third:
            f = 440.0
        elif i < 2 * third:
            f = 554.37
        else:
            f = 659.26
        local = i % third if i < 2 * third else i - 2 * third
        span = third if i < 2 * third else n - 2 * third
        s = 0.55 * math.sin(2 * math.pi * f * i / RATE)
        s += 0.25 * math.sin(2 * math.pi * f * 0.5 * i / RATE)
        s += 0.12 * math.sin(2 * math.pi * 110.0 * i / RATE)
        out.append(s * env(local, span, attack=0.03, curve=3.0))
    return out


def sh_lose():
    """The shoal scattered: a falling minor pair (E4 -> C4) sinking into
    a cold noise wash."""
    n = int(RATE * 0.5)
    half = n // 2
    noise = lcg(0xC0FFEE)
    low = 0.0
    out = []
    for i in range(n):
        t = i / n
        f = 329.63 if i < half else 261.63
        local = i if i < half else i - half
        span = half if i < half else n - half
        raw = next(noise)
        low = low + 0.05 * (raw - low)
        s = 0.6 * math.sin(2 * math.pi * f * i / RATE)
        s += 0.30 * low * min(1.0, t * 1.6)
        out.append(s * env(local, span, attack=0.02, curve=2.6))
    return out


if __name__ == '__main__':
    write_wav('sh_start.wav', sh_start())
    write_wav('sh_save.wav', sh_save())
    write_wav('sh_eaten.wav', sh_eaten())
    write_wav('sh_win.wav', sh_win())
    write_wav('sh_lose.wav', sh_lose())
