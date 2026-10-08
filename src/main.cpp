#include "duel/ai.hpp"
#include "duel/app.hpp"
#include "duel/replay.hpp"
#include <charconv>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string_view>

namespace {
std::uint32_t number(std::string_view text) {
    std::uint32_t result = 0;
    const auto parsed = std::from_chars(text.data(), text.data() + text.size(), result);
    if (parsed.ec != std::errc{} || parsed.ptr != text.data() + text.size())
        throw std::invalid_argument("Expected a non-negative integer: " + std::string(text));
    return result;
}
int smallNumber(std::string_view text) {
    const auto result = number(text);
    if (result > 100000) throw std::invalid_argument("Numeric option is too large");
    return static_cast<int>(result);
}
void summary(const duel::Simulation& simulation) {
    std::cout << "seed=" << simulation.config().seed << " rounds=" << simulation.round()
              << " shots=" << simulation.shots() << " hash=" << simulation.hash() << '\n';
    for (const auto& tank : simulation.tanks())
        std::cout << "player=" << tank.id + 1 << " wins=" << tank.wins << " damage=" << tank.damageDealt << '\n';
}
}

int main(int argc, char** argv) {
    try {
        duel::Options options;
        bool humansExplicit = false;
        options.config.seed = static_cast<std::uint32_t>(std::chrono::system_clock::now().time_since_epoch().count());
        for (int i = 1; i < argc; ++i) {
            const std::string arg = argv[i];
            auto value = [&]() -> std::string {
                if (++i >= argc) throw std::invalid_argument("Missing value for " + arg);
                return argv[i];
            };
            if (arg == "--help" || arg == "-h") {
                std::cout << "Ocelovy Duel - C++ artillery simulation\n\n"
                    "  --seed N                  Reproducible map and gameplay seed\n"
                    "  --players N               2-9 players (default 2)\n"
                    "  --humans N                First N players use local controls (default 1)\n"
                    "  --rounds N                1-9 rounds (default 3)\n"
                    "  --terrain hills|mountains|desert\n"
                    "  --difficulty easy|normal|hard\n"
                    "  --headless                Run an AI-only match without SDL\n"
                    "  --record FILE             Save completed shots as a replay\n"
                    "  --replay FILE             Watch and verify a replay\n"
                    "  --verify FILE             Verify replay headlessly\n"
                    "  --smoke FRAMES            Run GUI AI demo for a bounded number of frames\n"
                    "  --smoke-input FILE        Start at menu and inject scheduled key events\n"
                    "  --screenshot FILE.png     Capture the last smoke frame\n"
                    "  --capture DIRECTORY       Capture smoke frames at 20 fps for a demo video\n";
                return 0;
            } else if (arg == "--headless") options.headless = true;
            else if (arg == "--seed") options.config.seed = number(value());
            else if (arg == "--players") options.config.players = smallNumber(value());
            else if (arg == "--humans") { options.config.humans = smallNumber(value()); humansExplicit = true; }
            else if (arg == "--rounds") options.config.rounds = smallNumber(value());
            else if (arg == "--record") options.recordPath = value();
            else if (arg == "--replay") options.replayPath = value();
            else if (arg == "--verify") options.verifyPath = value();
            else if (arg == "--smoke") {
                options.smokeFrames = smallNumber(value());
                if (options.smokeFrames == 0) throw std::invalid_argument("Smoke frame count must be positive");
            } else if (arg == "--screenshot") options.screenshotPath = value();
            else if (arg == "--smoke-input") options.smokeInputPath = value();
            else if (arg == "--capture") options.captureDirectory = value();
            else if (arg == "--terrain") {
                const auto name = value();
                if (name == "hills") options.config.terrain = duel::TerrainStyle::Hills;
                else if (name == "mountains") options.config.terrain = duel::TerrainStyle::Mountains;
                else if (name == "desert") options.config.terrain = duel::TerrainStyle::Desert;
                else throw std::invalid_argument("Unknown terrain: " + name);
            } else if (arg == "--difficulty") {
                const auto name = value();
                if (name == "easy") options.config.difficulty = duel::Difficulty::Easy;
                else if (name == "normal") options.config.difficulty = duel::Difficulty::Normal;
                else if (name == "hard") options.config.difficulty = duel::Difficulty::Hard;
                else throw std::invalid_argument("Unknown difficulty: " + name);
            } else throw std::invalid_argument("Unknown option: " + arg);
        }
        if ((options.headless || options.smokeFrames) && !humansExplicit) options.config.humans = 0;
        options.config.validate();
        const int modes = static_cast<int>(options.headless) + static_cast<int>(!options.replayPath.empty()) +
            static_cast<int>(!options.verifyPath.empty());
        if (modes > 1) throw std::invalid_argument("Choose one of --headless, --replay, or --verify");
        if ((!options.screenshotPath.empty() || !options.captureDirectory.empty() || !options.smokeInputPath.empty()) && !options.smokeFrames)
            throw std::invalid_argument("Capture options require --smoke FRAMES");
        if ((options.headless || !options.verifyPath.empty()) && options.smokeFrames)
            throw std::invalid_argument("--smoke requires the GUI");
        if ((!options.replayPath.empty() || !options.verifyPath.empty()) && !options.recordPath.empty())
            throw std::invalid_argument("Recording and replay playback are separate modes");
        if (!options.verifyPath.empty()) {
            const auto replay = duel::loadReplay(options.verifyPath);
            const auto simulation = duel::verifyReplay(replay);
            std::cout << "Verified " << replay.shots.size() << " replay checkpoints\n";
            summary(simulation);
            return 0;
        }
        if (options.headless) {
            if (options.config.humans) throw std::invalid_argument("Headless matches require --humans 0");
            duel::Simulation simulation(options.config);
            duel::Replay replay{options.config, {}};
            std::uint64_t candidates = 0;
            const auto start = std::chrono::steady_clock::now();
            while (simulation.phase() != duel::Phase::MatchOver) {
                if (simulation.phase() == duel::Phase::RoundOver) { simulation.nextRound(); continue; }
                const auto decision = duel::chooseShot(simulation);
                candidates += static_cast<std::uint64_t>(decision.candidates);
                if (!simulation.fire(decision.command)) throw std::logic_error("AI shot rejected");
                simulation.resolveShot();
                replay.shots.push_back({simulation.round(), decision.command, simulation.hash()});
            }
            if (!options.recordPath.empty()) duel::saveReplay(options.recordPath, replay);
            summary(simulation);
            const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
            std::cout << "ai_candidates=" << candidates << " elapsed_ms=" << ms << '\n';
            return 0;
        }
#ifdef DUEL_WITH_SDL
        return duel::runFrontend(options);
#else
        throw std::runtime_error("This build has no GUI; use --headless or --verify, or rebuild with DUEL_BUILD_GUI=ON");
#endif
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
