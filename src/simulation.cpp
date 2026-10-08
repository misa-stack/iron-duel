#include "duel/core.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <stdexcept>

namespace duel {
namespace {
constexpr double Pi = 3.14159265358979323846;
constexpr std::array<WeaponSpec, 5> Weapons{{
    {"Shell", 30, 55, 1080, 0}, {"Rocket", 50, 65, 360, 0},
    {"Bouncer", 35, 65, 1080, 3}, {"Cluster", 24, 30, 1080, 0},
    {"Ultimate", 45, 60, 1080, 2}
}};

bool validCommand(const ShotCommand& command) {
    return std::isfinite(command.angle) && std::isfinite(command.power) &&
        command.angle >= 5 && command.angle <= 175 && command.power >= 5 && command.power <= 100 &&
        command.weapon >= Weapon::Shell && command.weapon < Weapon::Count;
}

Projectile launch(const Tank& tank, const ShotCommand& command) {
    const double angle = command.angle * Pi / 180;
    const Vec2 direction{std::cos(angle), -std::sin(angle)};
    return {{tank.x + 32 * direction.x, tank.y - 12 + 32 * direction.y},
            {8 * command.power * direction.x, 8 * command.power * direction.y}, command.weapon, tank.id, 0, 0};
}

struct StepResult { bool remove = false; bool impact = false; Vec2 position; };

// Both the live simulation and AI trajectory search call this exact integrator.
// Segment sampling at <= 1 pixel prevents fast shells skipping tanks or thin soil.
StepResult advanceProjectile(Projectile& projectile, const Terrain& terrain, const std::vector<Tank>& tanks) {
    ++projectile.age;
    if (projectile.age > 900) return {true, false, {}};
    const Vec2 from = projectile.position;
    const Vec2 delta{projectile.velocity.x * FixedDt, projectile.velocity.y * FixedDt};
    projectile.velocity.y += weaponSpec(projectile.weapon).gravity * FixedDt;
    const int samples = std::max(1, static_cast<int>(std::ceil(std::max(std::abs(delta.x), std::abs(delta.y)))));
    Vec2 previous = from;
    for (int i = 0; i <= samples; ++i) {
        const double ratio = static_cast<double>(i) / samples;
        const Vec2 point{from.x + delta.x * ratio, from.y + delta.y * ratio};
        if (point.x < 0 || point.x >= WorldWidth || point.y < -1500) return {true, false, {}};
        bool hitTank = false;
        for (const auto& tank : tanks) {
            if (!tank.alive() || (tank.id == projectile.owner && projectile.age < 5)) continue;
            const double dx = point.x - tank.x;
            const double dy = point.y - (tank.y - 10);
            if (dx * dx + dy * dy <= 18 * 18) { hitTank = true; break; }
        }
        const bool hitGround = terrain.solid(static_cast<int>(std::floor(point.x)), static_cast<int>(std::floor(point.y)));
        if (hitTank || hitGround) {
            projectile.position = point;
            if (!hitTank && point.y < WorldHeight && projectile.bounces < weaponSpec(projectile.weapon).bounces) {
                const int x = static_cast<int>(point.x);
                const double slope = (terrain.surface(std::min(WorldWidth - 1, x + 4)) -
                                      terrain.surface(std::max(0, x - 4))) / 8.0;
                const double length = std::sqrt(1 + slope * slope);
                const Vec2 normal{slope / length, -1 / length};
                const double dot = projectile.velocity.x * normal.x + projectile.velocity.y * normal.y;
                projectile.velocity.x = (projectile.velocity.x - 2 * dot * normal.x) * 0.7;
                projectile.velocity.y = (projectile.velocity.y - 2 * dot * normal.y) * 0.7;
                projectile.position = {previous.x + normal.x * 2, previous.y + normal.y * 2};
                ++projectile.bounces;
                return {false, projectile.weapon == Weapon::Ultimate, point};
            }
            return {true, true, point};
        }
        previous = point;
    }
    projectile.position = {from.x + delta.x, from.y + delta.y};
    return {};
}

class Hasher {
public:
    void add(std::uint64_t n) {
        for (int i = 0; i < 8; ++i) { value ^= n & 255u; value *= 1099511628211ull; n >>= 8; }
    }
    void real(double number) { std::uint64_t bits = 0; std::memcpy(&bits, &number, sizeof bits); add(bits); }
    std::uint64_t value = 14695981039346656037ull;
};
}

const WeaponSpec& weaponSpec(Weapon weapon) {
    if (weapon < Weapon::Shell || weapon >= Weapon::Count) throw std::invalid_argument("Invalid weapon");
    return Weapons[static_cast<std::size_t>(weapon)];
}
const char* phaseName(Phase phase) {
    switch (phase) {
    case Phase::Aiming: return "Aiming";
    case Phase::Flight: return "Shot in flight";
    case Phase::Explosions: return "Explosions";
    case Phase::Settling: return "Terrain settling";
    case Phase::RoundOver: return "Round over";
    case Phase::MatchOver: return "Match over";
    }
    return "Unknown";
}
const char* terrainName(TerrainStyle style) {
    return style == TerrainStyle::Hills ? "Hills" : (style == TerrainStyle::Mountains ? "Mountains" : "Desert");
}
const char* difficultyName(Difficulty difficulty) {
    return difficulty == Difficulty::Easy ? "Easy" : (difficulty == Difficulty::Normal ? "Normal" : "Hard");
}

Simulation::Simulation(Config config) : config_(config), terrain_(config.seed, config.terrain), random_(config.seed) {
    config.validate();
    tanks_.resize(static_cast<std::size_t>(config.players));
    for (int i = 0; i < config.players; ++i) {
        auto& tank = tanks_[static_cast<std::size_t>(i)];
        tank.id = i;
        tank.human = i < config.humans;
    }
    startRound();
}

void Simulation::startRound() {
    terrain_ = Terrain(config_.seed ^ (0x9e3779b9u * static_cast<std::uint32_t>(round_)),
                       static_cast<TerrainStyle>((static_cast<int>(config_.terrain) + round_ - 1) % 3));
    projectiles_.clear();
    explosions_.clear();
    winner_.reset();
    roundShots_ = 0;
    active_ = (round_ - 1) % config_.players;
    phase_ = Phase::Aiming;
    for (auto& tank : tanks_) {
        tank.x = (tank.id + 0.5) * WorldWidth / config_.players;
        tank.y = terrain_.surface(static_cast<int>(tank.x));
        tank.health = 100;
        tank.angle = tank.x < WorldWidth / 2 ? 45 : 135;
        tank.power = 65;
        tank.ammo = {{-1, 3, 3, 2, 1}};
        tank.deathExploded = false;
        tank.fallStartY = tank.y;
    }
}

bool Simulation::busy() const {
    return phase_ == Phase::Flight || phase_ == Phase::Explosions || phase_ == Phase::Settling;
}

bool Simulation::fire(const ShotCommand& command) {
    if (phase_ != Phase::Aiming || command.player != active_ || !validCommand(command)) return false;
    auto& tank = tanks_[static_cast<std::size_t>(active_)];
    auto& ammo = tank.ammo[static_cast<std::size_t>(command.weapon)];
    if (!tank.alive() || ammo == 0) return false;
    if (ammo > 0) --ammo;
    tank.angle = command.angle;
    tank.power = command.power;
    for (auto& t : tanks_) t.fallStartY = t.y;
    projectiles_.push_back(launch(tank, command));
    phase_ = Phase::Flight;
    ++shots_;
    ++roundShots_;
    return true;
}

void Simulation::explode(Vec2 point, Weapon weapon, int owner, bool fragments) {
    const auto& spec = weaponSpec(weapon);
    point.y = std::min(point.y, static_cast<double>(WorldHeight - 1));
    explosions_.push_back({point, spec.radius, 24});
    terrain_.crater(static_cast<int>(point.x), static_cast<int>(point.y), spec.radius);
    for (auto& tank : tanks_) {
        if (!tank.alive()) continue;
        const double distance = std::hypot(point.x - tank.x, point.y - (tank.y - 10));
        const int damage = std::min(tank.health, static_cast<int>(spec.damage * std::clamp(1 - distance / (spec.radius + 18), 0.0, 1.0)));
        tank.health -= damage;
        if (tank.id != owner && owner >= 0) tanks_[static_cast<std::size_t>(owner)].damageDealt += damage;
    }
    if (fragments && (weapon == Weapon::Cluster || weapon == Weapon::Ultimate)) {
        const int count = weapon == Weapon::Cluster ? 12 : 8;
        for (int i = 0; i < count && projectiles_.size() < 96; ++i) {
            const double angle = (15 + random_.bounded(151)) * Pi / 180;
            const double speed = 180 + random_.bounded(160);
            projectiles_.push_back({{point.x, point.y - 3}, {speed * std::cos(angle), -speed * std::sin(angle)},
                                    weapon == Weapon::Ultimate ? Weapon::Rocket : Weapon::Shell, owner, 0, 0});
        }
    }
}

void Simulation::deathExplosions(int owner) {
    // A chain reaction is finite: each stable player slot can explode only once.
    bool found = true;
    while (found) {
        found = false;
        for (auto& tank : tanks_) {
            if (!tank.alive() && !tank.deathExploded) {
                tank.deathExploded = true;
                explode({tank.x, tank.y - 10}, Weapon::Shell, owner, false);
                found = true;
            }
        }
    }
}

void Simulation::tick() {
    if (!busy()) return;
    ++ticks_;
    for (auto& blast : explosions_) --blast.ticksLeft;
    explosions_.erase(std::remove_if(explosions_.begin(), explosions_.end(),
                                     [](const Explosion& blast) { return blast.ticksLeft <= 0; }), explosions_.end());
    if (phase_ == Phase::Flight) {
        // Detach the current batch: fragmentation cannot invalidate its iterators.
        auto current = std::move(projectiles_);
        projectiles_.clear();
        projectiles_.reserve(current.size() + 16);
        for (auto projectile : current) {
            const auto result = advanceProjectile(projectile, terrain_, tanks_);
            if (!result.remove) projectiles_.push_back(projectile);
            if (result.impact) explode(result.position, projectile.weapon, projectile.owner, true);
        }
        deathExplosions(active_);
        if (projectiles_.empty()) phase_ = Phase::Explosions;
    } else if (phase_ == Phase::Explosions) {
        if (explosions_.empty()) phase_ = Phase::Settling;
    } else if (phase_ == Phase::Settling) {
        const bool settled = terrain_.settleStep();
        for (auto& tank : tanks_) if (tank.alive()) tank.y = terrain_.surface(static_cast<int>(tank.x));
        if (settled) {
            for (auto& tank : tanks_) {
                if (!tank.alive()) continue;
                const int damage = tank.y >= WorldHeight ? tank.health :
                    static_cast<int>(std::max(0.0, tank.y - tank.fallStartY - 12) / 2);
                const int applied = std::min(damage, tank.health);
                tank.health -= applied;
                if (tank.id != active_) tanks_[static_cast<std::size_t>(active_)].damageDealt += applied;
                tank.fallStartY = tank.y;
            }
            deathExplosions(active_);
            if (!explosions_.empty()) phase_ = Phase::Explosions;
            else finishTurn();
        }
    }
}

void Simulation::finishTurn() {
    const auto alive = std::count_if(tanks_.begin(), tanks_.end(), [](const Tank& tank) { return tank.alive(); });
    if (alive <= 1 || roundShots_ >= MaxRoundShots) {
        if (alive == 1) {
            const auto it = std::find_if(tanks_.begin(), tanks_.end(), [](const Tank& tank) { return tank.alive(); });
            winner_ = it->id;
            ++it->wins;
        }
        phase_ = round_ == config_.rounds ? Phase::MatchOver : Phase::RoundOver;
        return;
    }
    do { active_ = (active_ + 1) % config_.players; } while (!tanks_[static_cast<std::size_t>(active_)].alive());
    phase_ = Phase::Aiming;
}

bool Simulation::nextRound() {
    if (phase_ != Phase::RoundOver) return false;
    ++round_;
    startRound();
    return true;
}

void Simulation::resolveShot() {
    for (int i = 0; i < MaxShotTicks && busy(); ++i) tick();
    if (busy()) throw std::runtime_error("Shot exceeded simulation tick budget");
}

Trajectory Simulation::trace(const ShotCommand& command) const {
    Trajectory result;
    if (!validCommand(command) || command.player < 0 || command.player >= config_.players) return result;
    auto projectile = launch(tanks_[static_cast<std::size_t>(command.player)], command);
    result.points.push_back(projectile.position);
    for (int i = 0; i < 902; ++i) {
        const auto step = advanceProjectile(projectile, terrain_, tanks_);
        if (i % 6 == 0 || step.impact) result.points.push_back(projectile.position);
        if (step.impact) result.impacts.push_back(step.position);
        if (step.remove) break;
    }
    return result;
}

std::uint64_t Simulation::hash() const {
    Hasher hash;
    hash.add(config_.seed); hash.add(static_cast<unsigned>(config_.terrain));
    hash.add(static_cast<unsigned>(config_.difficulty)); hash.add(static_cast<unsigned>(config_.rounds));
    hash.add(terrain_.hash()); hash.add(random_.state());
    hash.add(static_cast<unsigned>(phase_)); hash.add(static_cast<unsigned>(active_));
    hash.add(static_cast<unsigned>(round_)); hash.add(static_cast<unsigned>(shots_));
    hash.add(static_cast<unsigned>(roundShots_)); hash.add(ticks_);
    hash.add(static_cast<unsigned>(winner_.value_or(-1)));
    for (const auto& tank : tanks_) {
        hash.add(static_cast<unsigned>(tank.id)); hash.add(tank.human); hash.real(tank.x); hash.real(tank.y);
        hash.real(tank.angle); hash.real(tank.power); hash.real(tank.fallStartY);
        hash.add(static_cast<unsigned>(tank.health)); hash.add(static_cast<unsigned>(tank.wins));
        hash.add(static_cast<unsigned>(tank.damageDealt)); hash.add(tank.deathExploded);
        for (int ammo : tank.ammo) hash.add(static_cast<unsigned>(ammo));
    }
    for (const auto& projectile : projectiles_) {
        hash.real(projectile.position.x); hash.real(projectile.position.y);
        hash.real(projectile.velocity.x); hash.real(projectile.velocity.y);
        hash.add(static_cast<unsigned>(projectile.weapon)); hash.add(static_cast<unsigned>(projectile.owner));
        hash.add(static_cast<unsigned>(projectile.age)); hash.add(static_cast<unsigned>(projectile.bounces));
    }
    for (const auto& blast : explosions_) {
        hash.real(blast.position.x); hash.real(blast.position.y);
        hash.add(static_cast<unsigned>(blast.radius)); hash.add(static_cast<unsigned>(blast.ticksLeft));
    }
    return hash.value;
}

} // namespace duel
