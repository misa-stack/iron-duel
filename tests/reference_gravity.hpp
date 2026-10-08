#pragma once
#include "duel/core.hpp"

// Deliberately simple, full-grid oracle matching the original bottom-up rule.
// Used only by correctness tests and the benchmark, never by the game.
inline bool referenceGravity(std::vector<std::uint8_t>& cells) {
    bool moved = false;
    for (int x = 0; x < duel::WorldWidth; ++x) {
        for (int y = duel::WorldHeight - 2; y >= 0; --y) {
            const auto i = static_cast<std::size_t>(x * duel::WorldHeight + y);
            if (cells[i] && !cells[i + 1]) {
                cells[i] = 0;
                cells[i + 1] = 1;
                moved = true;
            }
        }
    }
    return !moved;
}
