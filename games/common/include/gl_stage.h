/*
 * games/common — GENERIC staged progression (game-agnostic, header-only).
 *
 * Maps a scalar progress value (depth, distance, score, elapsed time) onto
 * a stage index via ascending thresholds, and reports transitions so the
 * caller can react exactly once per stage change (banner, palette swap,
 * difficulty step). Original code for the game-lab Track B lane; transfers
 * unchanged to any staged concept (Lumen Drift cave sections, Clockwork
 * Courier tower floors, Shoal level waves).
 */

#ifndef GL_STAGE_H
#define GL_STAGE_H

namespace gl
{

/// Stage tracker over `count` ascending thresholds: stage() is the number
/// of thresholds the last observed value has reached (0 before the first
/// threshold, `count` at/after the last). The thresholds array must outlive
/// the tracker and be sorted ascending.
class stage_track
{

public:
    constexpr stage_track(const int* thresholds, int count) :
        _thresholds(thresholds),
        _count(count)
    {
    }

    /// Current stage index in [0, count].
    [[nodiscard]] int stage() const
    {
        return _stage;
    }

    /// Recompute the stage from `value`; true exactly when the stage
    /// changed (in either direction — pass a high-water value if stages
    /// must never regress).
    bool update(int value)
    {
        int new_stage = 0;

        while(new_stage < _count && value >= _thresholds[new_stage])
        {
            ++new_stage;
        }

        if(new_stage == _stage)
        {
            return false;
        }

        _stage = new_stage;
        return true;
    }

    /// Back to stage 0 (e.g. on run restart).
    void reset()
    {
        _stage = 0;
    }

private:
    const int* _thresholds;
    int _count;
    int _stage = 0;
};

} // namespace gl

#endif
