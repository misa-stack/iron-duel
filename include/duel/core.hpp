#pragma once

#include <array>
#include <bitset>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace duel {

constexpr int WorldWidth = 1067;
constexpr int WorldHeight = 600;
constexpr double FixedDt = 1.0 / 60.0;
constexpr int MaxShotTicks = 2400;
constexpr int MaxRoundShots = 200;

struct Vec2 { double x = 0; double y = 0; };
enum class TerrainStyle { Hills, Mountains, Desert };
enum class Weapon { Shell, Rocket, Bouncer, Cluster, Ultimate, Count };
enum class Difficulty { Easy, Normal, Hard };
enum class Phase { Aiming, Flight, Explosions, Settling, RoundOver, MatchOver };

class Random {
public:
    explicit Random(std::uint32_t seed) : state_(seed ? seed : 0x6d2b79f5u) {}
    std::uint32_t next();
    int bounded(int bound);
    std::uint32_t state() const { return state_; }
private:
    std::uint32_t state_;
};

struct Config {
    std::uint32_t seed = 42;
    int players = 2;
    int humans = 1;
    int rounds = 3;
    TerrainStyle terrain = TerrainStyle::Hills;
    Difficulty difficulty = Difficulty::Normal;
    void validate() const;
};

// Column-major occupancy preserves caves and overhangs. Only changed columns
// participate in gravity; revision counters let the renderer cache their pixels.
class Terrain {
public:
    Terrain(std::uint32_t seed = 42, TerrainStyle style = TerrainStyle::Hills);
    static Terrain flat(int surface);
    bool solid(int x, int y) const;
    int surface(int x) const;
    int crater(int x, int y, int radius);
    bool settleStep(); // true once every affected column is at rest
    const std::vector<std::uint8_t>& cells() const { return cells_; }
    std::uint64_t revision(int x) const { return revisions_.at(static_cast<std::size_t>(x)); }
    std::uint64_t hash() const;
    std::size_t visitedCells() const { return visitedCells_; }
    TerrainStyle style() const { return style_; }
private:
    std::vector<std::uint8_t> cells_;
    std::array<int, WorldWidth> surfaces_{};
    std::array<std::uint64_t, WorldWidth> revisions_{};
    std::bitset<WorldWidth> unsettled_;
    TerrainStyle style_;
    std::size_t visitedCells_ = 0;
    void refreshSurface(int x);
};

struct WeaponSpec {
    const char* name;
    int radius;
    int damage;
    double gravity;
    int bounces;
};
const WeaponSpec& weaponSpec(Weapon weapon);
const char* phaseName(Phase phase);
const char* terrainName(TerrainStyle style);
const char* difficultyName(Difficulty difficulty);

struct Tank {
    int id = 0;
    bool human = true;
    double x = 0;
    double y = 0;
    double angle = 45;
    double power = 65;
    int health = 100;
    int wins = 0;
    int damageDealt = 0;
    std::array<int, 5> ammo{{-1, 3, 3, 2, 1}}; // -1: unlimited
    bool deathExploded = false;
    double fallStartY = 0;
    bool alive() const { return health > 0; }
};

struct ShotCommand {
    int player = 0;
    double angle = 45; // degrees above the right-facing horizontal, [5, 175]
    double power = 65; // percent, [5, 100]
    Weapon weapon = Weapon::Shell;
};

struct Projectile {
    Vec2 position;
    Vec2 velocity;
    Weapon weapon = Weapon::Shell;
    int owner = 0;
    int age = 0;
    int bounces = 0;
};
struct Explosion { Vec2 position; int radius; int ticksLeft; };
struct Trajectory { std::vector<Vec2> points; std::vector<Vec2> impacts; };

class Simulation {
public:
    explicit Simulation(Config config = {});
    const Config& config() const { return config_; }
    const Terrain& terrain() const { return terrain_; }
    const std::vector<Tank>& tanks() const { return tanks_; }
    const std::vector<Projectile>& projectiles() const { return projectiles_; }
    const std::vector<Explosion>& explosions() const { return explosions_; }
    Phase phase() const { return phase_; }
    int activePlayer() const { return active_; }
    int round() const { return round_; }
    int shots() const { return shots_; }
    int roundShots() const { return roundShots_; }
    std::optional<int> roundWinner() const { return winner_; }
    bool busy() const;
    bool fire(const ShotCommand& command);
    void tick(); // exactly one FixedDt; rendering and wall time have no role here
    bool nextRound();
    void resolveShot(); // bounded, headless equivalent of repeated tick()
    Trajectory trace(const ShotCommand& command) const;
    std::uint64_t hash() const;
private:
    Config config_;
    Terrain terrain_;
    Random random_;
    std::vector<Tank> tanks_;
    std::vector<Projectile> projectiles_;
    std::vector<Explosion> explosions_;
    Phase phase_ = Phase::Aiming;
    int active_ = 0;
    int round_ = 1;
    int shots_ = 0;
    int roundShots_ = 0;
    std::uint64_t ticks_ = 0;
    std::optional<int> winner_;
    void startRound();
    void explode(Vec2 position, Weapon weapon, int owner, bool fragments);
    void deathExplosions(int owner);
    void finishTurn();
};

// Accumulator shared by the UI and timing tests. Long stalls are capped to avoid
// unbounded catch-up; this may slow wall-clock progress, never enlarge a physics step.
class FixedClock {
public:
    int advance(double elapsed);
    void reset() { accumulator_ = 0; }
private:
    double accumulator_ = 0;
};

} // namespace duel
