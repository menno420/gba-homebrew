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

SH_CODE_IWRAM int sh_pred_update(sh_fish* fish, sh_pred* preds,
                                 unsigned run_frames)
{
    // The flock centroid (whole px) — the straggler metric's anchor.
    int cen_x = 0, cen_y = 0, at_sea = 0;

    for(int i = 0; i < sh_fish_count; ++i)
    {
        if(fish[i].saved == 0)
        {
            cen_x += fish[i].x / sh_fp;
            cen_y += fish[i].y / sh_fp;
            ++at_sea;
        }
    }

    if(at_sea == 0)
    {
        return 0;
    }

    cen_x /= at_sea;
    cen_y /= at_sea;

    // Re-lock on schedule (and whenever a target became invalid):
    // predator 0 takes the FARTHEST unsaved fish from the centroid,
    // predator 1 the runner-up. Squared distances, ties by index —
    // fully deterministic.
    bool relock = (run_frames % sh_pred_relock) == 0;

    for(int p = 0; p < sh_predators; ++p)
    {
        int t = preds[p].target;

        if(t >= 0 && fish[t].saved != 0)
        {
            preds[p].target = -1;
            relock = true;
        }
    }

    if(relock)
    {
        int best = -1, best_d2 = -1;
        int second = -1, second_d2 = -1;

        for(int i = 0; i < sh_fish_count; ++i)
        {
            if(fish[i].saved != 0)
            {
                continue;
            }

            int dx = fish[i].x / sh_fp - cen_x;
            int dy = fish[i].y / sh_fp - cen_y;
            int d2 = dx * dx + dy * dy;

            if(d2 > best_d2)
            {
                second = best;
                second_d2 = best_d2;
                best = i;
                best_d2 = d2;
            }
            else if(d2 > second_d2)
            {
                second = i;
                second_d2 = d2;
            }
        }

        // The straggler threshold: a fish inside the school's
        // sh_straggle_r2 ring is NOT prey — hunters starve on a tight
        // school; only the abandoned are taken.
        if(best >= 0 && best_d2 <= sh_straggle_r2)
        {
            best = -1;
        }

        if(second >= 0 && second_d2 <= sh_straggle_r2)
        {
            second = -1;
        }

        if(preds[0].cooldown == 0)
        {
            preds[0].target = best;
        }

        if(sh_predators > 1 && preds[1].cooldown == 0)
        {
            preds[1].target = second >= 0 ? second : -1;
        }
    }

    int newly_eaten = 0;

    for(int p = 0; p < sh_predators; ++p)
    {
        sh_pred& pr = preds[p];

        if(pr.cooldown > 0)
        {
            --pr.cooldown;               // digesting in the den
            pr.target = -1;
            continue;
        }

        int t = pr.target;

        if(t < 0)
        {
            continue;                    // waits for the next lock scan
        }

        // Stalk: per-axis clamped pursuit (the flock's own metric).
        int dx = fish[t].x - pr.x;
        int dy = fish[t].y - pr.y;
        int step_x = dx > sh_pred_speed ? sh_pred_speed
                   : (dx < -sh_pred_speed ? -sh_pred_speed : dx);
        int step_y = dy > sh_pred_speed ? sh_pred_speed
                   : (dy < -sh_pred_speed ? -sh_pred_speed : dy);
        pr.x += step_x;
        pr.y += step_y;

        int ex = pr.x / sh_fp - fish[t].x / sh_fp;
        int ey = pr.y / sh_fp - fish[t].y / sh_fp;

        if(ex * ex + ey * ey < sh_pred_eat_r2)
        {
            fish[t].saved = 2;           // EATEN
            fish[t].vx = 0;
            fish[t].vy = 0;
            ++newly_eaten;
            pr.x = sh_pred_den_x[p] * sh_fp;   // drag the kill home
            pr.y = sh_pred_den_y[p] * sh_fp;
            pr.cooldown = sh_pred_cooldown;
            pr.target = -1;
        }
    }

    return newly_eaten;
}

SH_CODE_IWRAM int sh_gate_update(sh_fish* fish)
{
    // STATIC GEOMETRY ONLY (the concept's "through gates"): no state,
    // no RNG — a pure position test per fish per wall, run AFTER the
    // flock pass so the walls have the last word on the frame. A fish
    // inside a wall band and outside that wall's gap is ejected to the
    // side it was swimming FROM (vx sign; a still fish is treated as
    // an east-facing one — deterministic either way) with its approach
    // damped. sh_speed_max (1.5 px/f) < the 5 px band means entry is
    // always caught before exit: no tunneling by construction.
    int blocked = 0;

    for(int i = 0; i < sh_fish_count; ++i)
    {
        sh_fish& f = fish[i];

        if(f.saved)
        {
            continue;
        }

        int px = f.x / sh_fp;
        int py = f.y / sh_fp;

        for(int g = 0; g < sh_gates; ++g)
        {
            if(px < sh_gate_x[g] - sh_gate_half
               || px > sh_gate_x[g] + sh_gate_half)
            {
                continue;
            }

            if(py >= sh_gate_gap_y0[g] && py <= sh_gate_gap_y1[g])
            {
                continue;                // through the gap — swim on
            }

            if(f.vx > 0)
            {
                f.x = (sh_gate_x[g] - sh_gate_half - 1) * sh_fp;
            }
            else
            {
                f.x = (sh_gate_x[g] + sh_gate_half + 1) * sh_fp;
            }

            f.vx = -(f.vx / 2);          // the coral gives nothing back
            ++blocked;
        }
    }

    return blocked;
}
