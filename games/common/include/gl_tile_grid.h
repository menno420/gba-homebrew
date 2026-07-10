/*
 * games/common — GENERIC tile-grid AABB collision (game-agnostic, header-only).
 *
 * Axis-separated collide-and-slide against a grid of solid cells. The grid's
 * content comes from a caller-supplied predicate, so ANY map representation
 * works (Lumen Drift's dynamic cave cells, Clockwork Courier's level tilemaps,
 * ...). Coordinates are map-space pixels: (0,0) = map top-left, +y down.
 * Original code for the game-lab Track B lane.
 */

#ifndef GL_TILE_GRID_H
#define GL_TILE_GRID_H

#include "bn_fixed.h"
#include "gl_kinematics.h"

namespace gl
{

/// Result bitmask from tile_grid::move.
constexpr int hit_x = 1; ///< horizontal motion was blocked this frame
constexpr int hit_y = 2; ///< vertical motion was blocked this frame

/// Collision grid over cells of `cell_size` pixels; `Solid` is a callable
/// `bool(int cell_x, int cell_y)` (out-of-range cells should report solid
/// if the map border must be impassable).
template<typename Solid>
class tile_grid
{

public:
    constexpr tile_grid(int cell_size, Solid solid) :
        _solid(solid),
        _cell_size(cell_size)
    {
    }

    /// True if the AABB centered at (x, y) with the given half extents
    /// overlaps any solid cell.
    [[nodiscard]] bool overlaps(bn::fixed x, bn::fixed y, bn::fixed half_w,
                                bn::fixed half_h) const
    {
        int min_cx = _cell(x - half_w);
        int max_cx = _cell(x + half_w - _epsilon);
        int min_cy = _cell(y - half_h);
        int max_cy = _cell(y + half_h - _epsilon);

        for(int cy = min_cy; cy <= max_cy; ++cy)
        {
            for(int cx = min_cx; cx <= max_cx; ++cx)
            {
                if(_solid(cx, cy))
                {
                    return true;
                }
            }
        }

        return false;
    }

    /// Move `b` by its velocity, axis-separated, clipping against solid
    /// cells (the blocked axis' velocity is zeroed and the body is snapped
    /// flush to the wall). Returns a hit_x/hit_y bitmask.
    int move(body& b, bn::fixed half_w, bn::fixed half_h) const
    {
        int result = 0;

        // Horizontal axis.
        bn::fixed new_x = b.pos.x() + b.vel.x();

        if(overlaps(new_x, b.pos.y(), half_w, half_h))
        {
            if(b.vel.x() > 0)
            {
                int cell = _cell(new_x + half_w - _epsilon);
                new_x = (cell * _cell_size) - half_w - _epsilon;
            }
            else if(b.vel.x() < 0)
            {
                int cell = _cell(new_x - half_w);
                new_x = ((cell + 1) * _cell_size) + half_w;
            }

            b.vel.set_x(0);
            result |= hit_x;
        }

        b.pos.set_x(new_x);

        // Vertical axis.
        bn::fixed new_y = b.pos.y() + b.vel.y();

        if(overlaps(b.pos.x(), new_y, half_w, half_h))
        {
            if(b.vel.y() > 0)
            {
                int cell = _cell(new_y + half_h - _epsilon);
                new_y = (cell * _cell_size) - half_h - _epsilon;
            }
            else if(b.vel.y() < 0)
            {
                int cell = _cell(new_y - half_h);
                new_y = ((cell + 1) * _cell_size) + half_h;
            }

            b.vel.set_y(0);
            result |= hit_y;
        }

        b.pos.set_y(new_y);
        return result;
    }

private:
    static constexpr bn::fixed _epsilon = bn::fixed::from_data(16);

    Solid _solid;
    int _cell_size;

    [[nodiscard]] int _cell(bn::fixed map_px) const
    {
        return (map_px / _cell_size).floor_integer();
    }
};

} // namespace gl

#endif
