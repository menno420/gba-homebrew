/*
 * games/common — GENERIC magic-checked SRAM save slot (game-agnostic,
 * header-only).
 *
 * Wraps bn::sram behind a tiny "does a valid save exist?" contract: the
 * payload is stored behind an 8-byte magic tag, so a fresh cartridge (SRAM
 * reads 0xFF), a corrupted save or another game's save all read as "no save"
 * instead of garbage scores. Bump the tag (e.g. "LDRIFT1" -> "LDRIFT2") to
 * invalidate old saves after a payload layout change.
 *
 * Original code for the game-lab Track B lane; transfers unchanged to any
 * concept that persists a small POD across power cycles (Lumen Drift best
 * depth, Clockwork Courier level unlocks, Shoal star ratings).
 */

#ifndef GL_SAVE_H
#define GL_SAVE_H

#include "bn_sram.h"

namespace gl
{

/// Magic-checked SRAM save slot at SRAM offset 0 for one trivially
/// copyable Payload. load() fills the payload only when the magic tag
/// matches; save() (re)writes tag + payload.
template<typename Payload>
class save_slot
{

public:
    static constexpr int magic_size = 8;

    /// @param magic NUL-terminated tag of at most 7 characters ("LDRIFT1").
    explicit save_slot(const char* magic)
    {
        for(int index = 0; index < magic_size; ++index)
        {
            _magic[index] = *magic;

            if(*magic)
            {
                ++magic;
            }
        }
    }

    /// Reads the payload from SRAM. Returns true and fills the payload if
    /// the magic tag matches (a previous save() exists); returns false and
    /// leaves the payload untouched otherwise (fresh/foreign/erased SRAM).
    [[nodiscard]] bool load(Payload& payload) const
    {
        record stored;
        bn::sram::read(stored);

        for(int index = 0; index < magic_size; ++index)
        {
            if(stored.magic[index] != _magic[index])
            {
                return false;
            }
        }

        payload = stored.payload;
        return true;
    }

    /// Writes tag + payload to SRAM (a later load() succeeds).
    void save(const Payload& payload)
    {
        record stored;

        for(int index = 0; index < magic_size; ++index)
        {
            stored.magic[index] = _magic[index];
        }

        stored.payload = payload;
        bn::sram::write(stored);
    }

private:
    struct record
    {
        char magic[magic_size];
        Payload payload;
    };

    static_assert(int(sizeof(record)) <= 32 * 1024, "Payload exceeds SRAM");

    char _magic[magic_size];
};

} // namespace gl

#endif
