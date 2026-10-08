#pragma once

#include "duel/core.hpp"

#include <iosfwd>

namespace duel {

struct RecordedShot { int round; ShotCommand command; std::uint64_t resultingHash; };
struct Replay { Config config; std::vector<RecordedShot> shots; };
void writeReplay(std::ostream& output, const Replay& replay);
Replay readReplay(std::istream& input);
void saveReplay(const std::string& path, const Replay& replay);
Replay loadReplay(const std::string& path);

class ReplayPlayer {
public:
    explicit ReplayPlayer(Replay replay);
    void tick();
    bool finished() const { return finished_; }
    const Simulation& simulation() const { return simulation_; }
    std::size_t completedShots() const { return next_; }
private:
    Replay replay_;
    Simulation simulation_;
    std::size_t next_ = 0;
    bool inFlight_ = false;
    bool finished_ = false;
};

Simulation verifyReplay(const Replay& replay);

} // namespace duel
