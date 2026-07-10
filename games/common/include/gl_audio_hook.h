/*
 * gl_audio_hook.h — in-ROM audio evidence hook for headless proofs
 * (GENERIC, game-agnostic).
 *
 * Screenshots cannot hear. A game counts every sound trigger into this
 * fixed-name volatile EWRAM array; the headless harness resolves the
 * unmangled `gl_audio_hook` symbol from the game's ELF and reads the
 * counters over the emulated bus every frame
 * (tools/headless-screenshot.py --elf/--watch/--assert-watch), asserting
 * them at frames where a sound must have fired vs stayed silent — paired
 * with mixer-memory evidence that the audio engine actually voiced them
 * (docs/capabilities.md documents the full method).
 *
 * Counters are CUMULATIVE by design: assertions on totals (== / >=) are
 * robust against small replay offsets, unlike exact fire-frame matching.
 * Slot meanings are game-defined; keep them stable per game.
 */

#ifndef GL_AUDIO_HOOK_H
#define GL_AUDIO_HOOK_H

extern "C"
{
    // One cumulative trigger counter per game-defined sound slot.
    inline volatile unsigned gl_audio_hook[8] = {};
}

namespace gl
{
    inline void count_audio(int slot)
    {
        gl_audio_hook[slot] = gl_audio_hook[slot] + 1;  // volatile-safe RMW
    }
}

#endif
