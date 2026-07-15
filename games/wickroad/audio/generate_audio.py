#!/usr/bin/env python3
"""Deterministic original sound effects for WICKROAD (growth cut 5 — the
concept's LAST cut).

The house pattern (games/lumen-drift/audio/generate_audio.py, carried by
Deepcast, Clockwork Courier and Shoal): a stdlib-only generator whose
output is byte-exact on every run (pure integer/float math, an explicit
LCG for noise — no `random` module, no timestamps), so the committed
WAVs can be regenerated and diffed. Butano's maxmod pipeline (mmutil)
converts the WAVs into the ROM soundbank at build time; the game plays
them as bn::sound_items::wr_*.

All three sounds are synthesized from scratch (sines, an LCG noise
source, exponential envelopes) — original audio, no samples, ever.

The road's palette (CONCEPT.md growth cut 5, verbatim): a COIN CHINK
when gold changes hands, a DAWN BELL when a day rolls over, and the
PASS-CLOSING WIND when winter takes the road. Everything is small and
dry — this is a candlelit ledger game, not an arcade.

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


def wr_coin():
    """Gold changes hands: a short bright metallic tick — two detuned
    high partials (near C7) plus a whisper of their octave, fast
    exponential decay. Fires on every buy/sell, so it must layer
    politely on a nine-buy streak: ~150 ms, dry."""
    n = int(RATE * 0.15)
    out = []
    for i in range(n):
        s = 0.48 * math.sin(2 * math.pi * 2093.0 * i / RATE)
        s += 0.34 * math.sin(2 * math.pi * 2217.5 * i / RATE)
        s += 0.14 * math.sin(2 * math.pi * 4186.0 * i / RATE)
        out.append(s * env(i, n, attack=0.002, curve=13.0))
    return out


def wr_dawn():
    """A day rolls over: one struck bell — inharmonic partials (the
    classic hum/prime/tierce spread: 1.0 / 2.756 / 5.404 over C5),
    the high partials dying first, ~750 ms, gentle. Rings on every
    dawn of a 30-day run, so it stays soft and round."""
    n = int(RATE * 0.75)
    f0 = 523.25
    out = []
    for i in range(n):
        t = i / n
        s = 0.46 * math.sin(2 * math.pi * f0 * i / RATE)
        s += 0.26 * math.sin(2 * math.pi * f0 * 2.756 * i / RATE) \
            * math.exp(-2.4 * t)
        s += 0.12 * math.sin(2 * math.pi * f0 * 5.404 * i / RATE) \
            * math.exp(-5.0 * t)
        out.append(s * env(i, n, attack=0.006, curve=4.2))
    return out


def wr_wind():
    """The pass closes: a low-mid wind swell — LCG noise through a
    one-pole lowpass whose cutoff gusts on a slow sine, under a
    rise-and-die swell window, ~1.1 s. The run's one loss sound:
    cold, not loud."""
    n = int(RATE * 1.1)
    noise = lcg(0x57494E44)              # 'WIND'
    low = 0.0
    out = []
    for i in range(n):
        t = i / n
        raw = next(noise)
        k = 0.05 + 0.04 * math.sin(2 * math.pi * 1.3 * t)
        low = low + k * (raw - low)      # gusting one-pole lowpass
        swell = math.sin(math.pi * min(1.0, t * 1.02)) ** 1.5
        s = 0.9 * low
        s += 0.08 * math.sin(2 * math.pi * 110.0 * i / RATE) * swell
        out.append(s * swell)
    return out


if __name__ == '__main__':
    write_wav('wr_coin.wav', wr_coin())
    write_wav('wr_dawn.wav', wr_dawn())
    write_wav('wr_wind.wav', wr_wind())
