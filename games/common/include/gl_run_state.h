/*
 * games/common — GENERIC arcade run lifecycle (game-agnostic, header-only).
 *
 * Minimal playing -> over -> restart state machine with an in-RAM
 * best-score high-water mark that survives restarts (not power cycles).
 * Original code for the game-lab Track B lane; transfers unchanged to any
 * run-based concept (Lumen Drift depth runs, Clockwork Courier level
 * attempts, Shoal rescue rounds).
 */

#ifndef GL_RUN_STATE_H
#define GL_RUN_STATE_H

#include "bn_math.h"

namespace gl
{

/// Run lifecycle: call fail(score) when the run ends, update() once per
/// frame while over (drives the restart-input grace period), restart() to
/// start the next run. best() persists across restarts in RAM.
class run_state
{

public:
    [[nodiscard]] bool playing() const
    {
        return ! _over;
    }

    [[nodiscard]] bool over() const
    {
        return _over;
    }

    /// Best final score across all runs this power-on (in-RAM only).
    [[nodiscard]] int best() const
    {
        return _best;
    }

    /// Frames elapsed since fail() — gate restart input on this so a button
    /// mashed at the moment of death doesn't skip the game-over screen.
    [[nodiscard]] int frames_over() const
    {
        return _frames_over;
    }

    /// Tick the over-state timer (no-op while playing).
    void update()
    {
        if(_over && _frames_over < 30000)
        {
            ++_frames_over;
        }
    }

    /// End the current run with its final score (idempotent until restart).
    void fail(int final_score)
    {
        if(! _over)
        {
            _over = true;
            _frames_over = 0;
            _best = bn::max(_best, final_score);
        }
    }

    /// Begin a new run (best score is kept).
    void restart()
    {
        _over = false;
        _frames_over = 0;
    }

private:
    bool _over = false;
    int _frames_over = 0;
    int _best = 0;
};

} // namespace gl

#endif
