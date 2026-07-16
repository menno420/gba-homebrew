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

// --- foragers: shortest dug path + nearest reachable patch (slice 3) --------
int32_t ur_patch_col(ur_patch_t p)
{
    int32_t col = p.x / UR_CELL;
    if (col < 0)
        col = 0;
    else if (col >= UR_GRID_W)
        col = UR_GRID_W - 1;
    return col;
}

uint32_t ur_dig_dist(const uint8_t grid[UR_CELLS], int32_t col, int32_t row)
{
    if (col < 0 || col >= UR_GRID_W || row < 0 || row >= UR_GRID_H)
        return UR_ROUTE_NONE;
    int32_t start = UR_ENTRANCE_ROW * UR_GRID_W + UR_ENTRANCE_COL;
    int32_t target = row * UR_GRID_W + col;
    if (grid[start] == 0 || grid[target] == 0)
        return UR_ROUTE_NONE;

    uint32_t dist[UR_CELLS];
    for (int32_t i = 0; i < UR_CELLS; i++)
        dist[i] = UR_ROUTE_NONE;

    // FIFO queue for a breadth-first sweep -> shortest path in an unweighted
    // grid (dug cells only). A ring buffer sized to the cell count.
    int32_t queue[UR_CELLS];
    int32_t head = 0, tail = 0;
    dist[start] = 0;
    queue[tail++] = start;

    static const int32_t dc[4] = {1, -1, 0, 0};
    static const int32_t dr[4] = {0, 0, 1, -1};

    while (head < tail)
    {
        int32_t i = queue[head++];
        if (i == target)
            return dist[i];
        int32_t c = i % UR_GRID_W;
        int32_t r = i / UR_GRID_W;
        for (int k = 0; k < 4; k++)
        {
            int32_t nc = c + dc[k];
            int32_t nr = r + dr[k];
            if (nc < 0 || nc >= UR_GRID_W || nr < 0 || nr >= UR_GRID_H)
                continue;
            int32_t j = nr * UR_GRID_W + nc;
            if (grid[j] == 1 && dist[j] == UR_ROUTE_NONE)
            {
                dist[j] = dist[i] + 1;
                queue[tail++] = j;
            }
        }
    }
    return dist[target]; // UR_ROUTE_NONE if never reached
}

ur_forage_t ur_forage(const uint8_t grid[UR_CELLS], uint32_t seed, uint32_t season)
{
    ur_forage_t best;
    best.index = UR_ROUTE_NONE;
    best.dist = UR_ROUTE_NONE;
    best.route = UR_ROUTE_NONE;

    for (uint32_t i = 0; i < UR_PATCH_COUNT; i++)
    {
        ur_patch_t p = ur_patch(seed, season, i);
        int32_t col = ur_patch_col(p);
        uint32_t d = ur_dig_dist(grid, col, UR_DROP_ROW);
        // Reachable and strictly nearer than the best so far (ascending scan
        // keeps the lowest index on a tie).
        if (d != UR_ROUTE_NONE && (best.dist == UR_ROUTE_NONE || d < best.dist))
        {
            best.index = i;
            best.dist = d;
            best.route = d * 2u;
        }
    }
    return best;
}

// --- granaries: the food store (slice 4) ------------------------------------
int ur_designate(const uint8_t grid[UR_CELLS], uint8_t gran[UR_CELLS],
                 int32_t col, int32_t row)
{
    if (col < 0 || col >= UR_GRID_W || row < 0 || row >= UR_GRID_H)
        return 0;
    int32_t i = row * UR_GRID_W + col;
    if (grid[i] == 0)                // only a DUG cell can hold a granary
        return 0;
    int changed = gran[i] == 0 ? 1 : 0;
    gran[i] = 1;
    return changed;
}

void ur_fresh_gran(uint8_t gran[UR_CELLS])
{
    for (int32_t i = 0; i < UR_CELLS; i++)
        gran[i] = 0;
}

uint32_t ur_gran_count(const uint8_t grid[UR_CELLS], const uint8_t gran[UR_CELLS])
{
    uint32_t n = 0;
    for (int32_t i = 0; i < UR_CELLS; i++)
        n += (gran[i] && grid[i]) ? 1u : 0u;
    return n;
}

uint32_t ur_gran_connected(const uint8_t grid[UR_CELLS], const uint8_t gran[UR_CELLS])
{
    // A designated cell banks only if it is CONNECTED to the mouth — the same
    // reachability the burrow BFS defines (finite ur_dig_dist). Reusing
    // ur_dig_dist keeps storage reachability and forager reachability the ONE
    // meaning of the dug graph (the slice-3 cross-layer guard).
    uint32_t n = 0;
    for (int32_t r = 0; r < UR_GRID_H; r++)
        for (int32_t c = 0; c < UR_GRID_W; c++)
        {
            int32_t i = r * UR_GRID_W + c;
            if (gran[i] && grid[i] && ur_dig_dist(grid, c, r) != UR_ROUTE_NONE)
                n++;
        }
    return n;
}

uint32_t ur_gran_capacity(const uint8_t grid[UR_CELLS], const uint8_t gran[UR_CELLS])
{
    return ur_gran_connected(grid, gran) * (uint32_t)UR_GRAN_CAP;
}

uint32_t ur_reachable_food(const uint8_t grid[UR_CELLS], uint32_t seed, uint32_t season)
{
    uint32_t total = 0;
    for (uint32_t i = 0; i < UR_PATCH_COUNT; i++)
    {
        ur_patch_t p = ur_patch(seed, season, i);
        int32_t col = ur_patch_col(p);
        if (ur_dig_dist(grid, col, UR_DROP_ROW) != UR_ROUTE_NONE)
            total += (uint32_t)p.amount;
    }
    return total;
}

uint32_t ur_store(const uint8_t grid[UR_CELLS], const uint8_t gran[UR_CELLS],
                  uint32_t seed, uint32_t season)
{
    uint32_t food = ur_reachable_food(grid, seed, season);
    uint32_t cap = ur_gran_capacity(grid, gran);
    return food < cap ? food : cap;  // deposits bank up to capacity
}

// --- nurseries + population (slice 5) ---------------------------------------
void ur_fresh_nurs(uint8_t nurs[UR_CELLS])
{
    for (int32_t i = 0; i < UR_CELLS; i++)
        nurs[i] = 0;
}

uint32_t ur_nurse(const uint8_t grid[UR_CELLS], uint8_t nurs[UR_CELLS],
                  uint32_t col, uint32_t row)
{
    if (col >= UR_GRID_W || row >= UR_GRID_H)
        return 0;
    uint32_t i = row * UR_GRID_W + col;
    if (grid[i] == 0)                // only a DUG cell can hold a nursery
        return 0;
    uint32_t changed = nurs[i] == 0 ? 1u : 0u;
    nurs[i] = 1;
    return changed;
}

uint32_t ur_nurs_count(const uint8_t grid[UR_CELLS], const uint8_t nurs[UR_CELLS])
{
    uint32_t n = 0;
    for (int32_t i = 0; i < UR_CELLS; i++)
        n += (nurs[i] && grid[i]) ? 1u : 0u;
    return n;
}

uint32_t ur_nurs_connected(const uint8_t grid[UR_CELLS], const uint8_t nurs[UR_CELLS])
{
    // A designated nursery broods only if it is CONNECTED to the mouth — the
    // same reachability the burrow BFS defines (finite ur_dig_dist). Reusing
    // ur_dig_dist keeps broodkeeping reachability, storage reachability and
    // forager reachability the ONE meaning of the dug graph.
    uint32_t n = 0;
    for (int32_t r = 0; r < UR_GRID_H; r++)
        for (int32_t c = 0; c < UR_GRID_W; c++)
        {
            int32_t i = r * UR_GRID_W + c;
            if (nurs[i] && grid[i] && ur_dig_dist(grid, c, r) != UR_ROUTE_NONE)
                n++;
        }
    return n;
}

uint32_t ur_pop(const uint8_t grid[UR_CELLS], const uint8_t gran[UR_CELLS],
                const uint8_t nurs[UR_CELLS], uint32_t seed, uint32_t season)
{
    uint32_t store = ur_store(grid, gran, seed, season);
    uint32_t ncon  = ur_nurs_connected(grid, nurs);
    uint32_t brood = ncon * UR_NURS_BROOD;
    uint32_t food  = store / UR_FOOD_PER_ANT;
    uint32_t pop   = brood < food ? brood : food;
    if (pop > UR_POP_CAP) pop = UR_POP_CAP;
    return pop;
}

uint32_t ur_pop_food(const uint8_t grid[UR_CELLS], const uint8_t gran[UR_CELLS],
                     const uint8_t nurs[UR_CELLS], uint32_t seed, uint32_t season)
{
    return ur_pop(grid, gran, nurs, seed, season) * UR_FOOD_PER_ANT;
}

// --- hawks predate: exposed shallow route cells (slice 6) -------------------
uint32_t ur_route_exposed(const uint8_t grid[UR_CELLS], int32_t col, int32_t row)
{
    if (col < 0 || col >= UR_GRID_W || row < 0 || row >= UR_GRID_H)
        return 0;
    int32_t start = UR_ENTRANCE_ROW * UR_GRID_W + UR_ENTRANCE_COL;
    int32_t target = row * UR_GRID_W + col;
    if (grid[start] == 0 || grid[target] == 0)
        return 0;

    // BFS over the SAME 4-connected dug graph ur_dig_dist walks, recording the
    // predecessor each cell was FIRST reached from (E,W,S,N neighbour order) —
    // so the reconstructed path is the canonical shortest path, a deterministic
    // integer both host and ROM compute identically.
    int32_t pred[UR_CELLS];
    for (int32_t i = 0; i < UR_CELLS; i++)
        pred[i] = -1;

    int32_t queue[UR_CELLS];
    int32_t head = 0, tail = 0;
    pred[start] = start;            // the mouth is its own root (also = visited)
    queue[tail++] = start;

    static const int32_t dc[4] = {1, -1, 0, 0};
    static const int32_t dr[4] = {0, 0, 1, -1};

    int found = 0;
    while (head < tail)
    {
        int32_t i = queue[head++];
        if (i == target)
        {
            found = 1;
            break;
        }
        int32_t c = i % UR_GRID_W;
        int32_t r = i / UR_GRID_W;
        for (int k = 0; k < 4; k++)
        {
            int32_t nc = c + dc[k];
            int32_t nr = r + dr[k];
            if (nc < 0 || nc >= UR_GRID_W || nr < 0 || nr >= UR_GRID_H)
                continue;
            int32_t j = nr * UR_GRID_W + nc;
            if (grid[j] == 1 && pred[j] == -1)
            {
                pred[j] = i;
                queue[tail++] = j;
            }
        }
    }
    if (!found)
        return 0;

    // Walk the canonical path target->mouth, counting EXPOSED cells (grid row
    // < UR_SAFE_DEPTH). Both endpoints are on the path.
    uint32_t exposed = 0;
    int32_t cur = target;
    while (1)
    {
        int32_t r = cur / UR_GRID_W;
        if (r < UR_SAFE_DEPTH)
            exposed++;
        if (cur == start)
            break;
        cur = pred[cur];
    }
    return exposed;
}

uint32_t ur_forage_exposed(const uint8_t grid[UR_CELLS], uint32_t seed, uint32_t season)
{
    ur_forage_t f = ur_forage(grid, seed, season);
    if (f.index == UR_ROUTE_NONE)
        return 0;
    ur_patch_t p = ur_patch(seed, season, f.index);
    int32_t col = ur_patch_col(p);
    return ur_route_exposed(grid, col, UR_DROP_ROW);
}

uint32_t ur_predation(const uint8_t grid[UR_CELLS], const uint8_t gran[UR_CELLS],
                      const uint8_t nurs[UR_CELLS], uint32_t seed, uint32_t season)
{
    uint32_t pop = ur_pop(grid, gran, nurs, seed, season);
    uint32_t exposed = ur_forage_exposed(grid, seed, season);
    return exposed < pop ? exposed : pop;   // min: capped by the population
}

uint32_t ur_survivors(const uint8_t grid[UR_CELLS], const uint8_t gran[UR_CELLS],
                      const uint8_t nurs[UR_CELLS], uint32_t seed, uint32_t season)
{
    // predation <= pop, so this never underflows.
    return ur_pop(grid, gran, nurs, seed, season)
         - ur_predation(grid, gran, nurs, seed, season);
}
