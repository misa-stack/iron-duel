#include "duel/ai.hpp"
#include "duel/replay.hpp"
#include "reference_gravity.hpp"
#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>

using namespace duel;
namespace {
int checks = 0;
void check(bool condition, const char* expression, int line) {
    ++checks;
    if (!condition) throw std::runtime_error("Line " + std::to_string(line) + ": " + expression);
}
#define CHECK(x) check((x), #x, __LINE__)
template<typename F> void throws(F action) {
    bool caught = false;
    try { action(); } catch (const std::exception&) { caught = true; }
    CHECK(caught);
}

void terrainTests() {
    Terrain a(7), b(7), c(8);
    CHECK(a.hash() == b.hash());
    CHECK(a.hash() != c.hash());
    CHECK(!a.solid(-1, 500));
    CHECK(!a.solid(WorldWidth, WorldHeight));
    CHECK(!a.solid(0, -1));
    CHECK(a.solid(0, WorldHeight));
    CHECK(a.surface(-1) == WorldHeight);
    CHECK(a.surface(WorldWidth) == WorldHeight);
    CHECK(a.crater(std::numeric_limits<int>::min(), 0, 10) == 0);
    CHECK(a.crater(0, std::numeric_limits<int>::max(), 10) == 0);
    CHECK(a.settleStep());
    CHECK(a.visitedCells() == 0);
    for (int style = 0; style < 3; ++style) {
        Terrain terrain(91, static_cast<TerrainStyle>(style));
        // Overlapping craters, both edges, and underground voids exercise gravity.
        terrain.crater(0, 450, 35);
        terrain.crater(WorldWidth - 1, 540, 45);
        terrain.crater(515, 500, 40);
        terrain.crater(535, 480, 30);
        auto expected = terrain.cells();
        bool settled = false;
        for (int step = 0; step < WorldHeight && !settled; ++step) {
            const bool referenceDone = referenceGravity(expected);
            settled = terrain.settleStep();
            CHECK(settled == referenceDone);
            CHECK(terrain.cells() == expected);
        }
        CHECK(settled);
        CHECK(terrain.settleStep());
        CHECK(terrain.visitedCells() == 0);
        for (int x : {0, 515, 535, WorldWidth - 1}) {
            int height = 0;
            while (height < WorldHeight && !terrain.solid(x, height)) ++height;
            CHECK(terrain.surface(x) == height);
        }
    }
    auto empty = Terrain::flat(WorldHeight);
    CHECK(empty.surface(500) == WorldHeight);
    CHECK(empty.crater(500, 300, 40) == 0);
    throws([] { Terrain::flat(-1); });
}

void simulationTests() {
    Simulation simulation;
    const auto initial = simulation.hash();
    CHECK(!simulation.nextRound());
    CHECK(!simulation.fire({1, 45, 65, Weapon::Shell}));
    CHECK(!simulation.fire({0, 0, 65, Weapon::Shell}));
    CHECK(!simulation.fire({0, 45, std::numeric_limits<double>::quiet_NaN(), Weapon::Shell}));
    CHECK(!simulation.fire({0, 45, 65, Weapon::Count}));
    CHECK(simulation.hash() == initial);
    simulation.tick();
    CHECK(simulation.hash() == initial);
    for (int weapon = 0; weapon < 5; ++weapon) {
        Simulation game;
        const ShotCommand command{0, 25, 40, static_cast<Weapon>(weapon)};
        CHECK(game.fire(command));
        CHECK(!game.fire(command));
        game.resolveShot();
        CHECK(!game.busy());
        CHECK(game.projectiles().empty());
        CHECK(game.explosions().empty());
        CHECK(game.tanks().size() == 2);
        CHECK(game.shots() == 1);
        if (weapon > 0) CHECK(game.tanks()[0].ammo[static_cast<std::size_t>(weapon)] == Simulation().tanks()[0].ammo[static_cast<std::size_t>(weapon)] - 1);
    }
    // Zero hits cannot wedge a match forever: the 200-shot rule produces a draw.
    Config drawConfig;
    drawConfig.rounds = 1;
    Simulation draw(drawConfig);
    for (int i = 0; i < MaxRoundShots; ++i) {
        const auto id = draw.activePlayer();
        const double angle = id == 0 ? 175 : 5;
        CHECK(draw.fire({id, angle, 100, Weapon::Shell}));
        draw.resolveShot();
    }
    CHECK(draw.phase() == Phase::MatchOver);
    CHECK(!draw.roundWinner());
    CHECK(!draw.fire({draw.activePlayer(), 45, 50, Weapon::Shell}));
}

void timingTests() {
    std::uint64_t expected = 0;
    for (int fps : {30, 60, 144}) {
        Simulation simulation;
        CHECK(simulation.fire({0, 62, 80, Weapon::Cluster}));
        FixedClock clock;
        int ticks = 0;
        for (int frame = 0; frame < fps * 20; ++frame) {
            const int steps = clock.advance(1.0 / fps);
            ticks += steps;
            for (int i = 0; i < steps; ++i) simulation.tick();
        }
        CHECK(ticks == 1200);
        CHECK(!simulation.busy());
        if (!expected) expected = simulation.hash();
        CHECK(simulation.hash() == expected);
    }
    FixedClock clock;
    CHECK(clock.advance(-1) == 0);
    CHECK(clock.advance(std::numeric_limits<double>::infinity()) == 0);
    CHECK(clock.advance(5) == 15);
}

Replay aiMatch(Config config) {
    Simulation simulation(config);
    Replay replay{config, {}};
    while (simulation.phase() != Phase::MatchOver) {
        if (simulation.phase() == Phase::RoundOver) {
            CHECK(simulation.nextRound());
            CHECK(simulation.tanks().size() == static_cast<std::size_t>(config.players));
            for (const auto& tank : simulation.tanks()) CHECK(tank.health == 100);
            continue;
        }
        const auto before = simulation.hash();
        auto decision = chooseShot(simulation);
        CHECK(simulation.hash() == before);
        CHECK(decision.candidates > 0);
        CHECK(decision.fullSimulations > 0);
        CHECK(simulation.fire(decision.command));
        simulation.resolveShot();
        replay.shots.push_back({simulation.round(), decision.command, simulation.hash()});
        for (const auto& tank : simulation.tanks()) CHECK(tank.health >= 0 && tank.health <= 100);
    }
    CHECK(simulation.round() == config.rounds);
    CHECK(simulation.shots() <= config.rounds * MaxRoundShots);
    CHECK(verifyReplay(replay).hash() == simulation.hash());
    return replay;
}

void aiAndReplayTests() {
    Config config;
    config.humans = 0;
    config.rounds = 2;
    config.difficulty = Difficulty::Easy;
    auto replay = aiMatch(config);
    CHECK(!replay.shots.empty());
    std::stringstream stream;
    writeReplay(stream, replay);
    const auto loaded = readReplay(stream);
    CHECK(loaded.shots.size() == replay.shots.size());
    CHECK(verifyReplay(loaded).hash() == replay.shots.back().resultingHash);
    Config crowded = config;
    crowded.players = 9;
    crowded.rounds = 1;
    const auto crowdedReplay = aiMatch(crowded);
    CHECK(verifyReplay(crowdedReplay).tanks().size() == 9);
    auto changed = replay;
    changed.shots[0].resultingHash ^= 1;
    throws([&] { verifyReplay(changed); });
    changed = replay;
    changed.shots[0].command.player = 1;
    throws([&] { verifyReplay(changed); });
    changed = replay;
    changed.shots[0].round = 2;
    throws([&] { verifyReplay(changed); });
    std::stringstream empty;
    writeReplay(empty, {config, {}});
    CHECK(verifyReplay(readReplay(empty)).hash() == Simulation(config).hash());
    for (const std::string text : {"", "OCELOVY_DUEL_REPLAY 2\n", "OCELOVY_DUEL_REPLAY 1\n-1 2 0 1 0 0\n0\n",
        "OCELOVY_DUEL_REPLAY 1\n42 2 0 1 0 0\n200000\n", "OCELOVY_DUEL_REPLAY 1\n42 2 0 1 0 0\n0\nextra"}) {
        throws([&] { std::stringstream bad(text); readReplay(bad); });
    }
    config.rounds = 1;
    config.difficulty = Difficulty::Hard;
    Simulation game(config);
    auto decision = chooseShot(game);
    AiSearch incremental(game);
    while (!incremental.advance(1)) {}
    CHECK(decision.command.angle == incremental.decision().command.angle);
    CHECK(decision.command.power == incremental.decision().command.power);
    CHECK(decision.command.weapon == incremental.decision().command.weapon);
    CHECK(game.fire(decision.command));
    game.resolveShot();
    CHECK(game.tanks()[1].health < 100); // the bot must land a useful opening shot
    // Restarting replaces all state, including score, ammunition and seed stream.
    for (int i = 0; i < 20; ++i) {
        game = Simulation(config);
        CHECK(game.hash() == Simulation(config).hash());
        CHECK(game.fire({0, 20, 30, Weapon::Ultimate}));
        game.resolveShot();
    }
}
}

int main() {
    try {
        terrainTests(); std::cout << "PASS terrain and reference gravity\n";
        simulationTests(); std::cout << "PASS commands, weapons and round limits\n";
        timingTests(); std::cout << "PASS render-rate independence\n";
        aiAndReplayTests(); std::cout << "PASS AI, replays and restarts\n";
        std::cout << checks << " checks passed\n";
    } catch (const std::exception& error) { std::cerr << "FAIL " << error.what() << '\n'; return 1; }
}
