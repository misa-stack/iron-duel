#include "duel/core.hpp"
#include "../tests/reference_gravity.hpp"
#include <chrono>
#include <iostream>
#include <stdexcept>

int main() {
    using Clock = std::chrono::steady_clock;
    auto initial = duel::Terrain::flat(300);
    initial.crater(533, 365, 45);
    auto reference = initial.cells();
    auto optimized = initial;
    std::size_t fullVisits = 0, dirtyVisits = 0;
    int fullTicks = 0, dirtyTicks = 0;
    const auto startFull = Clock::now();
    bool fullDone = false;
    while (!fullDone) {
        fullDone = referenceGravity(reference);
        fullVisits += duel::WorldWidth * (duel::WorldHeight - 1);
        ++fullTicks;
    }
    const auto endFull = Clock::now();
    const auto startDirty = Clock::now();
    bool dirtyDone = false;
    while (!dirtyDone) {
        dirtyDone = optimized.settleStep();
        dirtyVisits += optimized.visitedCells();
        ++dirtyTicks;
    }
    const auto endDirty = Clock::now();
    if (reference != optimized.cells() || fullTicks != dirtyTicks)
        throw std::runtime_error("Gravity benchmark results differ");
    std::cout << "terrain=1067x600 crater_radius=45 ticks=" << dirtyTicks << " identical_final_cells=yes\n"
              << "full_grid_us=" << std::chrono::duration_cast<std::chrono::microseconds>(endFull - startFull).count()
              << " visited_cells=" << fullVisits << '\n'
              << "dirty_columns_us=" << std::chrono::duration_cast<std::chrono::microseconds>(endDirty - startDirty).count()
              << " visited_cells=" << dirtyVisits << '\n'
              << "cell_visit_reduction=" << static_cast<double>(fullVisits) / static_cast<double>(dirtyVisits) << "x\n";
}
