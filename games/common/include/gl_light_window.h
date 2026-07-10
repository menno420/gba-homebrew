/*
 * gl_light_window.h — circular "light radius" driver over the GBA's
 * hardware window (GENERIC, game-agnostic).
 *
 * Wraps Butano's internal rect window + a per-scanline H-Blank effect into
 * one call: set(center, radius) shows the wrapped backgrounds only inside a
 * circle of `radius` pixels around `center` (screen coordinates, origin at
 * the display center). Everything outside the circle falls back to the
 * backdrop color; sprites are untouched (they stay visible everywhere), so
 * HUD text and the player keep rendering over the darkness.
 *
 * The caller decides which backgrounds the light reveals:
 *
 *     gl::light_window light;
 *     bn::window::outside().set_show_bg(cave_bg, false);  // dark outside
 *     ...
 *     light.set(mote_screen_pos, radius);                 // every frame
 *
 * Cost: the half-width-per-scanline table is rebuilt only when the integer
 * radius changes (one integer sqrt per covered line); the per-frame work is
 * a 160-entry pair rebuild + the H-Blank effect reload. Fully deterministic
 * — the window affects rendering only, never physics.
 */

#ifndef GL_LIGHT_WINDOW_H
#define GL_LIGHT_WINDOW_H

#include "bn_array.h"
#include "bn_display.h"
#include "bn_fixed_point.h"
#include "bn_math.h"
#include "bn_rect_window.h"
#include "bn_utility.h"
#include "bn_rect_window_boundaries_hbe_ptr.h"

namespace gl
{

class light_window
{

public:
    /// Radius that keeps the whole screen lit from any on-screen center
    /// (> sqrt(120^2 + 80^2), the display's corner distance).
    [[nodiscard]] static constexpr int full_radius()
    {
        return 200;
    }

    light_window() :
        _window(bn::rect_window::internal()),
        _hbe(bn::rect_window_boundaries_hbe_ptr::create_horizontal(_window, _deltas))
    {
        // Vertical bounds stay wide open; the per-line horizontal deltas
        // carve the circle (a zero-width line shows nothing).
        _window.set_boundaries(-bn::display::height() / 2, 0,
                               bn::display::height() / 2, 0);
        set(bn::fixed_point(0, 0), full_radius());
    }

    /// Places the light circle: `center` in screen coordinates (origin at
    /// the display center), `radius` in pixels (clamped to [0, full]).
    void set(const bn::fixed_point& center, int radius)
    {
        radius = bn::clamp(radius, 0, full_radius());

        if(radius != _radius)
        {
            _radius = radius;

            int max_dy = bn::min(radius, bn::display::height());

            for(int dy = 0; dy <= max_dy; ++dy)
            {
                _half_widths[dy] = uint8_t(bn::min(
                        bn::sqrt(radius * radius - dy * dy), 160));
            }
        }

        int cx = center.x().round_integer();
        int cy = center.y().round_integer() + bn::display::height() / 2;

        for(int line = 0; line < bn::display::height(); ++line)
        {
            int dy = bn::abs(line - cy);
            int hw = dy <= _radius ? _half_widths[dy] : 0;
            _deltas[line] = bn::pair<bn::fixed, bn::fixed>(cx - hw, cx + hw);
        }

        _hbe.reload_deltas_ref();
    }

    [[nodiscard]] int radius() const
    {
        return _radius;
    }

private:
    bn::array<bn::pair<bn::fixed, bn::fixed>, bn::display::height()> _deltas;
    bn::array<uint8_t, bn::display::height() + 1> _half_widths;
    bn::rect_window _window;
    bn::rect_window_boundaries_hbe_ptr _hbe;
    int _radius = -1;
};

}

#endif
