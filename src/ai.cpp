#include "duel/ai.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace duel {

AiSearch::AiSearch(const Simulation& simulation) : snapshot_(simulation) {
    if (simulation.phase() != Phase::Aiming) throw std::invalid_argument("AI requires an aiming state");
    const auto difficulty = simulation.config().difficulty;
    const int angles = difficulty == Difficulty::Easy ? 9 : (difficulty == Difficulty::Normal ? 17 : 25);
    const int powers = difficulty == Difficulty::Easy ? 6 : (difficulty == Difficulty::Normal ? 10 : 14);
    const auto& tank = simulation.tanks()[static_cast<std::size_t>(simulation.activePlayer())];
    for (int weapon = 0; weapon < 5; ++weapon) {
        if (tank.ammo[static_cast<std::size_t>(weapon)] == 0) continue;
        for (int a = 0; a < angles; ++a)
            for (int p = 0; p < powers; ++p)
                commands_.push_back({tank.id, 5.0 + 170.0 * a / (angles - 1), 5.0 + 95.0 * p / (powers - 1), static_cast<Weapon>(weapon)});
    }
}

double AiSearch::approximate(const ShotCommand& command, const Trajectory& path) const {
    const auto& spec = weaponSpec(command.weapon);
    double score = -1000;
    double damageScore = 0;
    for (const auto& point : path.impacts) {
        for (const auto& tank : snapshot_.tanks()) {
            if (!tank.alive()) continue;
            const double distance = std::hypot(point.x - tank.x, point.y - (tank.y - 10));
            const double damage = spec.damage * std::clamp(1 - distance / (spec.radius + 18), 0.0, 1.0);
            if (tank.id == command.player) damageScore -= damage * 2.5;
            else { damageScore += damage; score = std::max(score, -distance * 0.035); }
        }
    }
    // A small cost preserves scarce ammunition when the expected result is similar.
    return score + damageScore - static_cast<int>(command.weapon) * 0.12;
}

void AiSearch::consider(const ShotCommand& command) {
    auto path = snapshot_.trace(command);
    const double score = approximate(command, path);
    ++decision_.candidates;
    best_.push_back({command, score});
    std::stable_sort(best_.begin(), best_.end(), [](const Candidate& a, const Candidate& b) { return a.score > b.score; });
    const std::size_t limit = snapshot_.config().difficulty == Difficulty::Easy ? 2 :
        (snapshot_.config().difficulty == Difficulty::Normal ? 4 : 6);
    if (best_.size() > limit) best_.resize(limit);
    if (score >= best_.front().score) {
        preview_.push_back(std::move(path));
        if (preview_.size() > 10) preview_.erase(preview_.begin());
    }
}

bool AiSearch::advance(int candidateBudget) {
    if (candidateBudget < 1) throw std::invalid_argument("AI budget must be positive");
    if (done_) return true;
    for (int i = 0; i < candidateBudget && next_ < commands_.size(); ++i) consider(commands_[next_++]);
    if (next_ < commands_.size()) return false;
    if (!refined_) {
        refined_ = true;
        // Refine around the strongest coarse candidates, retaining the original grid
        // results so refinement can never discard a better legal shot.
        const auto coarse = best_;
        for (const auto& candidate : coarse) {
            for (int a = -2; a <= 2; ++a) {
                for (int p = -2; p <= 2; ++p) {
                    if (a == 0 && p == 0) continue;
                    auto command = candidate.command;
                    command.angle = std::clamp(command.angle + a * 1.5, 5.0, 175.0);
                    command.power = std::clamp(command.power + p * 1.5, 5.0, 100.0);
                    commands_.push_back(command);
                }
            }
        }
        return false;
    }
    if (finalist_ < best_.size()) {
        const auto& candidate = best_[finalist_++];
        auto trial = snapshot_;
        if (!trial.fire(candidate.command)) throw std::logic_error("AI produced an illegal shot");
        trial.resolveShot();
        ++decision_.fullSimulations;
        double score = candidate.score * 0.01;
        for (std::size_t i = 0; i < trial.tanks().size(); ++i) {
            const auto& before = snapshot_.tanks()[i];
            const auto& after = trial.tanks()[i];
            const double damage = before.health - after.health;
            const double kill = before.alive() && !after.alive() ? 100 : 0;
            score += (static_cast<int>(i) == candidate.command.player ? -2.5 : 1.0) * (damage + kill);
        }
        if (score > bestExactScore_) {
            bestExactScore_ = score;
            decision_.command = candidate.command;
            decision_.score = score;
        }
        return false;
    }
    // Difficulty noise has its own reproducible stream and never consumes game RNG.
    Random random(snapshot_.config().seed ^ (static_cast<std::uint32_t>(snapshot_.shots()) + 1u) * 0x85ebca6bu);
    const int error = snapshot_.config().difficulty == Difficulty::Easy ? 5 :
        (snapshot_.config().difficulty == Difficulty::Normal ? 1 : 0);
    decision_.command.angle = std::clamp(decision_.command.angle + random.bounded(error * 2 + 1) - error, 5.0, 175.0);
    decision_.command.power = std::clamp(decision_.command.power + random.bounded(error * 2 + 1) - error, 5.0, 100.0);
    preview_.push_back(snapshot_.trace(decision_.command));
    done_ = true;
    return true;
}

const AiDecision& AiSearch::decision() const {
    if (!done_) throw std::logic_error("AI search is not finished");
    return decision_;
}

AiDecision chooseShot(const Simulation& simulation) {
    AiSearch search(simulation);
    while (!search.advance(256)) {}
    return search.decision();
}

} // namespace duel
