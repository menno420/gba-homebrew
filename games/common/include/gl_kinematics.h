/*
 * games/common — GENERIC fixed-point kinematics (game-agnostic, header-only).
 *
 * A minimal Euler-integrated body: accumulate accelerations, clamp, damp.
 * Original code for the game-lab Track B lane; reusable by any concept with
 * moving actors (mote, courier robot, fish boids).
 */

#ifndef GL_KINEMATICS_H
#define GL_KINEMATICS_H

#include "bn_math.h"
#include "bn_fixed_point.h"

namespace gl
{

/// Point body with position + velocity in pixel units (bn::fixed).
struct body
{
    bn::fixed_point pos;
    bn::fixed_point vel;

    /// Add an acceleration (per-frame velocity delta).
    void accelerate(const bn::fixed_point& acc)
    {
        vel += acc;
    }

    /// Clamp each velocity component to [-max, +max].
    void clamp_velocity(bn::fixed max_x, bn::fixed max_y)
    {
        vel.set_x(bn::clamp(vel.x(), -max_x, max_x));
        vel.set_y(bn::clamp(vel.y(), -max_y, max_y));
    }

    /// Multiply horizontal velocity by a damping factor (drag).
    void damp_x(bn::fixed factor)
    {
        vel.set_x(vel.x() * factor);
    }

    /// Multiply vertical velocity by a damping factor (drag).
    void damp_y(bn::fixed factor)
    {
        vel.set_y(vel.y() * factor);
    }
};

} // namespace gl

#endif
