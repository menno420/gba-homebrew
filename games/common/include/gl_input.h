/*
 * games/common — GENERIC input helpers (game-agnostic, header-only).
 *
 * Original code for the game-lab Track B lane; transfers unchanged to any
 * concept that reads the D-pad as an analog-ish steering/force vector
 * (Lumen Drift lateral drift, Clockwork Courier movement, Shoal current).
 */

#ifndef GL_INPUT_H
#define GL_INPUT_H

#include "bn_keypad.h"
#include "bn_fixed_point.h"

namespace gl
{

/// D-pad state as a direction vector with component values in {-1, 0, 1}.
/// Scale by a force/speed constant at the call site.
[[nodiscard]] inline bn::fixed_point dpad_vector()
{
    bn::fixed x = 0;
    bn::fixed y = 0;

    if(bn::keypad::left_held())
    {
        x -= 1;
    }

    if(bn::keypad::right_held())
    {
        x += 1;
    }

    if(bn::keypad::up_held())
    {
        y -= 1;
    }

    if(bn::keypad::down_held())
    {
        y += 1;
    }

    return bn::fixed_point(x, y);
}

} // namespace gl

#endif
