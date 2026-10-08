#include "duel/core.hpp"

#include <algorithm>
#include <cmath>
#include <functional>
#include <stdexcept>

namespace duel {

std::uint32_t Random::next() {
    state_ ^= state_ << 13;
    state_ ^= state_ >> 17;
    state_ ^= state_ << 5;
    return state_;
}

int Random::bounded(int bound) {
    if (bound <= 0) throw std::invalid_argument("Random bound must be positive");
    return static_cast<int>(next() % static_cast<std::uint32_t>(bound));
}

void Config::validate() const {
    if (players < 2 || players > 9 || humans < 0 || humans > players || rounds < 1 || rounds > 9)
        throw std::invalid_argument("Use 2-9 players, 0-players humans, and 1-9 rounds");
    if (terrain < TerrainStyle::Hills || terrain > TerrainStyle::Desert ||
        difficulty < Difficulty::Easy || difficulty > Difficulty::Hard)
        throw std::invalid_argument("Invalid terrain or difficulty");
}

Terrain::Terrain(std::uint32_t seed, TerrainStyle style)
    : cells_(WorldWidth * WorldHeight, 0), style_(style) {
    Random random(seed);
    // Midpoint displacement, retaining the prototype's three terrain families.
    const int roughness = style == TerrainStyle::Mountains ? 3 : (style == TerrainStyle::Hills ? 7 : 15);
    std::function<void(int, int, int, int)> subdivide = [&](int left, int ly, int right, int ry) {
        if (right - left <= 1) { surfaces_[static_cast<std::size_t>(left)] = ly; return; }
        const int middle = (left + right) / 2;
        const int spread = std::max(2, (right - left) / roughness);
        const int height = std::clamp((ly + ry) / 2 + random.bounded(spread * 2 + 1) - spread, 210, 530);
        subdivide(left, ly, middle, height);
        subdivide(middle, height, right, ry);
    };
    subdivide(0, 300 + random.bounded(170), WorldWidth, 300 + random.bounded(170));
    for (int x = 0; x < WorldWidth; ++x) {
        for (int y = surfaces_[static_cast<std::size_t>(x)]; y < WorldHeight; ++y)
            cells_[static_cast<std::size_t>(x * WorldHeight + y)] = 1;
        revisions_[static_cast<std::size_t>(x)] = 1;
    }
}

Terrain Terrain::flat(int surface) {
    if (surface < 0 || surface > WorldHeight) throw std::invalid_argument("Invalid terrain height");
    Terrain result;
    for (int x = 0; x < WorldWidth; ++x) {
        result.surfaces_[static_cast<std::size_t>(x)] = surface;
        for (int y = 0; y < WorldHeight; ++y)
            result.cells_[static_cast<std::size_t>(x * WorldHeight + y)] = static_cast<std::uint8_t>(y >= surface);
    }
    return result;
}

bool Terrain::solid(int x, int y) const {
    if (x < 0 || x >= WorldWidth || y < 0) return false;
    return y >= WorldHeight || cells_[static_cast<std::size_t>(x * WorldHeight + y)] != 0;
}

int Terrain::surface(int x) const {
    return x < 0 || x >= WorldWidth ? WorldHeight : surfaces_[static_cast<std::size_t>(x)];
}

void Terrain::refreshSurface(int x) {
    int y = 0;
    const auto start = static_cast<std::size_t>(x * WorldHeight);
    while (y < WorldHeight && cells_[start + static_cast<std::size_t>(y)] == 0) ++y;
    surfaces_[static_cast<std::size_t>(x)] = y;
}

int Terrain::crater(int cx, int cy, int radius) {
    if (radius < 0 || radius > WorldWidth) throw std::invalid_argument("Invalid crater radius");
    // Widen before arithmetic: callers may supply off-map coordinates.
    const auto left = std::max<std::int64_t>(0, static_cast<std::int64_t>(cx) - radius);
    const auto right = std::min<std::int64_t>(WorldWidth - 1, static_cast<std::int64_t>(cx) + radius);
    int removed = 0;
    for (auto x = left; x <= right; ++x) {
        const auto dx = x - cx;
        const int extent = static_cast<int>(std::sqrt(static_cast<double>(radius * radius - dx * dx)));
        bool changed = false;
        const auto top = std::max<std::int64_t>(0, static_cast<std::int64_t>(cy) - extent);
        const auto bottom = std::min<std::int64_t>(WorldHeight - 1, static_cast<std::int64_t>(cy) + extent);
        for (auto y = top; y <= bottom; ++y) {
            auto& cell = cells_[static_cast<std::size_t>(x * WorldHeight + y)];
            if (cell) { cell = 0; ++removed; changed = true; }
        }
        if (changed) {
            const auto column = static_cast<std::size_t>(x);
            unsettled_.set(column);
            ++revisions_[column];
            refreshSurface(static_cast<int>(x));
        }
    }
    return removed;
}

bool Terrain::settleStep() {
    visitedCells_ = 0;
    for (int x = 0; x < WorldWidth; ++x) {
        const auto column = static_cast<std::size_t>(x);
        if (!unsettled_.test(column)) continue;
        bool moved = false;
        const auto offset = static_cast<std::size_t>(x * WorldHeight);
        for (int y = WorldHeight - 2; y >= 0; --y) {
            ++visitedCells_;
            const auto index = offset + static_cast<std::size_t>(y);
            if (cells_[index] && !cells_[index + 1]) {
                cells_[index] = 0;
                cells_[index + 1] = 1;
                moved = true;
            }
        }
        if (moved) { ++revisions_[column]; refreshSurface(x); }
        else unsettled_.reset(column);
    }
    return unsettled_.none();
}

std::uint64_t Terrain::hash() const {
    std::uint64_t value = 14695981039346656037ull;
    for (auto cell : cells_) { value ^= cell; value *= 1099511628211ull; }
    return value;
}

int FixedClock::advance(double elapsed) {
    if (!std::isfinite(elapsed) || elapsed < 0) return 0;
    accumulator_ += std::min(elapsed, 0.25);
    const int steps = static_cast<int>((accumulator_ + 1e-12) / FixedDt);
    accumulator_ = std::max(0.0, accumulator_ - steps * FixedDt);
    return steps;
}

} // namespace duel
