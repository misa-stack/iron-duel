#pragma once

#include "duel/core.hpp"

namespace duel {

struct AiDecision {
    ShotCommand command;
    double score = 0;
    int candidates = 0;
    int fullSimulations = 0;
};

// Incremental search keeps event processing responsive. A cheap trajectory pass
// ranks the grid; finalists run the complete damage/fragment/gravity simulation.
class AiSearch {
public:
    explicit AiSearch(const Simulation& simulation);
    bool advance(int candidateBudget = 32);
    bool done() const { return done_; }
    const AiDecision& decision() const;
    const std::vector<Trajectory>& preview() const { return preview_; }
private:
    struct Candidate { ShotCommand command; double score; };
    Simulation snapshot_;
    std::vector<ShotCommand> commands_;
    std::vector<Candidate> best_;
    std::vector<Trajectory> preview_;
    std::size_t next_ = 0;
    std::size_t finalist_ = 0;
    bool refined_ = false;
    bool done_ = false;
    AiDecision decision_;
    double bestExactScore_ = -1e30;
    double approximate(const ShotCommand& command, const Trajectory& path) const;
    void consider(const ShotCommand& command);
};

AiDecision chooseShot(const Simulation& simulation);

} // namespace duel
