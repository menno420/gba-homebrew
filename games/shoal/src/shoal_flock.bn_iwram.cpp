/*
 * SHOAL — the flock pass, compiled ARM into IWRAM (Butano's
 * *.bn_iwram.cpp hot-loop rule). See shoal_flock.h for the measured
 * ladder that put it here: the branchy sim fetched from wait-stated
 * ROM was the whale, not the pair count.
 */

#include "shoal_flock.h"

SH_CODE_IWRAM int sh_flock_update(sh_fish* fish, int cursor_x,
                                  int cursor_y, bool pushing)
{
    int px_w[sh_fish_count];
    int py_w[sh_fish_count];

    for(int i = 0; i < sh_fish_count; ++i)
    {
        px_w[i] = fish[i].x / sh_fp;
        py_w[i] = fish[i].y / sh_fp;
    }

    // Bucket the school: 24px cells (= the sight radius), 3x3 scan
    // covers every possible neighbor. Caps are a deterministic rail
    // (fixed iteration order; an overflow fish goes unseen by LATER
    // fish this frame).
    constexpr int cell_px = 24;
    constexpr int grid_w = 10;           // covers 0..239
    constexpr int grid_h = 7;            // covers 0..167
    constexpr int bucket_cap = 24;
    unsigned char bucket_n[grid_h][grid_w] = {};
    unsigned char bucket[grid_h][grid_w][bucket_cap];

    for(int i = 0; i < sh_fish_count; ++i)
    {
        if(fish[i].saved)
        {
            continue;
        }

        int bx = px_w[i] / cell_px;
        int by = py_w[i] / cell_px;

        if(bucket_n[by][bx] < bucket_cap)
        {
            bucket[by][bx][bucket_n[by][bx]++] = (unsigned char)i;
        }
    }

    int newly_saved = 0;

    for(int i = 0; i < sh_fish_count; ++i)
    {
        sh_fish& f = fish[i];

        if(f.saved)
        {
            continue;
        }

        int sum_x = 0, sum_y = 0, sum_vx = 0, sum_vy = 0;
        int sep_x = 0, sep_y = 0;
        int neighbors = 0;

        int bx = px_w[i] / cell_px;
        int by = py_w[i] / cell_px;
        int by0 = by > 0 ? by - 1 : 0;
        int by1 = by < grid_h - 1 ? by + 1 : grid_h - 1;
        int bx0 = bx > 0 ? bx - 1 : 0;
        int bx1 = bx < grid_w - 1 ? bx + 1 : grid_w - 1;

        for(int cy = by0; cy <= by1 && neighbors < sh_neighbor_cap; ++cy)
        for(int cx = bx0; cx <= bx1 && neighbors < sh_neighbor_cap; ++cx)
        for(int k = 0; k < bucket_n[cy][cx]; ++k)
        {
            int j = bucket[cy][cx][k];

            if(j == i)
            {
                continue;
            }

            int dx = px_w[j] - px_w[i];
            int dy = py_w[j] - py_w[i];
            int d2 = dx * dx + dy * dy;

            if(d2 < sh_see_r2)
            {
                sum_x += px_w[j];
                sum_y += py_w[j];
                sum_vx += fish[j].vx;
                sum_vy += fish[j].vy;
                ++neighbors;

                if(d2 < sh_sep_r2)
                {
                    sep_x -= dx;
                    sep_y -= dy;
                }

                if(neighbors >= sh_neighbor_cap)
                {
                    break;
                }
            }
        }

        if(neighbors > 0)
        {
            // Cohesion toward the neighbor centroid, alignment toward
            // the average heading (px -> 8.8 units via the shifts).
            f.vx += ((sum_x / neighbors - px_w[i]) * sh_fp)
                    >> sh_coh_shift >> 8;
            f.vy += ((sum_y / neighbors - py_w[i]) * sh_fp)
                    >> sh_coh_shift >> 8;
            f.vx += (sum_vx / neighbors - f.vx) >> sh_ali_shift;
            f.vy += (sum_vy / neighbors - f.vy) >> sh_ali_shift;
        }

        f.vx += sep_x * sh_sep_gain;
        f.vy += sep_y * sh_sep_gain;

        // The current: A pushes the water outward around the cursor.
        if(pushing)
        {
            int dx = px_w[i] - cursor_x;
            int dy = py_w[i] - cursor_y;
            int d2 = dx * dx + dy * dy;

            if(d2 < sh_push_r2)
            {
                f.vx += (dx * sh_push_gain) >> 4;
                f.vy += (dy * sh_push_gain) >> 4;
            }
        }

        // Walls breathe the school back inward.
        if(px_w[i] < sh_water_x0 + sh_wall_margin) { f.vx += sh_wall_push; }
        if(px_w[i] > sh_water_x1 - sh_wall_margin) { f.vx -= sh_wall_push; }
        if(py_w[i] < sh_water_y0 + sh_wall_margin) { f.vy += sh_wall_push; }
        if(py_w[i] > sh_water_y1 - sh_wall_margin) { f.vy -= sh_wall_push; }

        // Clamp (per-axis) and keep a lazy minimum drift.
        if(f.vx > sh_speed_max) { f.vx = sh_speed_max; }
        if(f.vx < -sh_speed_max) { f.vx = -sh_speed_max; }
        if(f.vy > sh_speed_max) { f.vy = sh_speed_max; }
        if(f.vy < -sh_speed_max) { f.vy = -sh_speed_max; }

        if(f.vx > -sh_drift_min && f.vx < sh_drift_min)
        {
            f.vx += (f.vx >= 0) ? 2 : -2;
        }

        f.x += f.vx;
        f.y += f.vy;

        // Hard clamp to the water (the walls above make this rare).
        if(f.x < sh_water_x0 * sh_fp) { f.x = sh_water_x0 * sh_fp; }
        if(f.x > sh_water_x1 * sh_fp) { f.x = sh_water_x1 * sh_fp; }
        if(f.y < sh_water_y0 * sh_fp) { f.y = sh_water_y0 * sh_fp; }
        if(f.y > sh_water_y1 * sh_fp) { f.y = sh_water_y1 * sh_fp; }

        // The reef: settle and be counted.
        if(f.x >= sh_reef_x * sh_fp)
        {
            f.saved = 1;
            f.vx = 0;
            f.vy = 0;
            ++newly_saved;
        }
    }

    return newly_saved;
}
