// Underroot — pure simulation layer (arc slice 1). See ur_sim.h.
//
// MIRROR RULE: tools/check-underroot.py mirrors every function here
// line-for-line. Change one, change both, same PR.

#include "ur_sim.h"

uint32_t ur_hash(uint32_t a, uint32_t b)
{
    uint32_t h = (a * 0x9E3779B9u) ^ (b * 0x85EBCA6Bu);
    h ^= h >> 16;
    h *= 0x7FEB352Du;
    h ^= h >> 15;
    h *= 0x846CA68Bu;
    h ^= h >> 16;
    return h;
}

// --- hawk schedule ----------------------------------------------------------
uint32_t ur_hawk_pass(uint32_t frame)
{
    return frame / UR_HAWK_PERIOD;
}

int ur_hawk_active(uint32_t frame)
{
    return (frame % UR_HAWK_PERIOD) < UR_HAWK_SWEEP ? 1 : 0;
}

uint32_t ur_hawk_dir(uint32_t seed, uint32_t season, uint32_t index)
{
    // 0 = left->right, 1 = right->left
    return ur_hash(ur_hash(seed, season), index) & 1u;
}

int32_t ur_hawk_x(uint32_t seed, uint32_t season, uint32_t frame)
{
    uint32_t t = frame % UR_HAWK_PERIOD;
    if (t >= UR_HAWK_SWEEP)
        return -1;
    uint32_t d = ur_hawk_dir(seed, season, ur_hawk_pass(frame));
    return d ? (int32_t)(UR_HAWK_SWEEP - 1u - t) : (int32_t)t;
}

int32_t ur_hawk_y(uint32_t seed, uint32_t season, uint32_t frame)
{
    uint32_t idx = ur_hawk_pass(frame);
    uint32_t span = UR_MEADOW_Y1 - UR_MEADOW_Y0;
    uint32_t h = ur_hash(ur_hash(seed ^ UR_HAWK_SALT, season), idx);
    return (int32_t)(UR_MEADOW_Y0 + (h % span));
}

// --- food patches -----------------------------------------------------------
ur_patch_t ur_patch(uint32_t seed, uint32_t season, uint32_t index)
{
    uint32_t h = ur_hash(ur_hash(seed ^ UR_PATCH_SALT, season), index);
    ur_patch_t p;
    uint32_t xspan = (uint32_t)(UR_APRON_X1 - UR_APRON_X0);
    uint32_t yspan = (uint32_t)(UR_APRON_Y1 - UR_APRON_Y0);
    uint32_t aspan = (uint32_t)(UR_PATCH_MAX - UR_PATCH_MIN + 1);
    p.x = UR_APRON_X0 + (int32_t)((h >> 8) % xspan);
    p.y = UR_APRON_Y0 + (int32_t)((h >> 20) % yspan);
    p.amount = UR_PATCH_MIN + (int32_t)(h % aspan);
    return p;
}

uint32_t ur_patch_total(uint32_t seed, uint32_t season)
{
    uint32_t total = 0;
    for (uint32_t i = 0; i < UR_PATCH_COUNT; i++)
        total += (uint32_t)ur_patch(seed, season, i).amount;
    return total;
}

// --- dig grid ---------------------------------------------------------------
int ur_cell_of_touch(int32_t tx, int32_t ty, int32_t *col, int32_t *row)
{
    if (tx < 0 || ty < 0 || tx >= UR_GRID_W * UR_CELL || ty >= UR_GRID_H * UR_CELL)
        return 0;
    *col = tx / UR_CELL;
    *row = ty / UR_CELL;
    return 1;
}

int ur_dig(uint8_t grid[UR_CELLS], int32_t col, int32_t row)
{
    int32_t i = row * UR_GRID_W + col;
    int changed = grid[i] == 0 ? 1 : 0;
    grid[i] = 1;
    return changed;
}

void ur_fresh_grid(uint8_t grid[UR_CELLS])
{
    for (int32_t i = 0; i < UR_CELLS; i++)
        grid[i] = 0;
    grid[UR_ENTRANCE_ROW * UR_GRID_W + UR_ENTRANCE_COL] = 1;
}

uint32_t ur_burrow_size(const uint8_t grid[UR_CELLS])
{
    int32_t start = UR_ENTRANCE_ROW * UR_GRID_W + UR_ENTRANCE_COL;
    if (grid[start] == 0)
        return 0;

    uint8_t seen[UR_CELLS];
    for (int32_t i = 0; i < UR_CELLS; i++)
        seen[i] = 0;

    int32_t stack[UR_CELLS];
    int32_t sp = 0;
    stack[sp++] = start;
    seen[start] = 1;
    uint32_t count = 0;

    static const int32_t dc[4] = {1, -1, 0, 0};
    static const int32_t dr[4] = {0, 0, 1, -1};

    while (sp > 0)
    {
        int32_t i = stack[--sp];
        count++;
        int32_t c = i % UR_GRID_W;
        int32_t r = i / UR_GRID_W;
        for (int k = 0; k < 4; k++)
        {
            int32_t nc = c + dc[k];
            int32_t nr = r + dr[k];
            if (nc < 0 || nc >= UR_GRID_W || nr < 0 || nr >= UR_GRID_H)
                continue;
            int32_t j = nr * UR_GRID_W + nc;
            if (!seen[j] && grid[j] == 1)
            {
                seen[j] = 1;
                stack[sp++] = j;
            }
        }
    }
    return count;
}
