#include "duel/replay.hpp"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace duel {
namespace {
std::string line(std::istream& input) {
    std::string result;
    // Bound line size before allocating an untrusted replay record.
    for (int i = 0; i < 512; ++i) {
        const int ch = input.get();
        if (ch == '\n') return result;
        if (ch == std::char_traits<char>::eof()) throw std::runtime_error("Truncated replay");
        if (ch != '\r') result.push_back(static_cast<char>(ch));
    }
    throw std::runtime_error("Replay line exceeds 512 bytes");
}
void endOfRecord(std::istringstream& input) {
    if (!input) throw std::runtime_error("Malformed replay record");
    input >> std::ws;
    if (!input.eof()) throw std::runtime_error("Unexpected replay fields");
}
}

void writeReplay(std::ostream& output, const Replay& replay) {
    replay.config.validate();
    output << "OCELOVY_DUEL_REPLAY 1\n";
    const auto& c = replay.config;
    output << c.seed << ' ' << c.players << ' ' << c.humans << ' ' << c.rounds << ' '
           << static_cast<int>(c.terrain) << ' ' << static_cast<int>(c.difficulty) << '\n';
    output << replay.shots.size() << '\n' << std::setprecision(std::numeric_limits<double>::max_digits10);
    for (const auto& shot : replay.shots)
        output << shot.round << ' ' << shot.command.player << ' ' << shot.command.angle << ' '
               << shot.command.power << ' ' << static_cast<int>(shot.command.weapon) << ' ' << shot.resultingHash << '\n';
    if (!output) throw std::runtime_error("Could not write replay");
}

Replay readReplay(std::istream& input) {
    if (line(input) != "OCELOVY_DUEL_REPLAY 1") throw std::runtime_error("Unsupported replay format or version");
    Replay replay;
    int style = 0, difficulty = 0;
    std::uint64_t seed = 0;
    auto header = std::istringstream(line(input));
    header >> seed >> replay.config.players >> replay.config.humans >> replay.config.rounds >> style >> difficulty;
    endOfRecord(header);
    if (seed > std::numeric_limits<std::uint32_t>::max() || style < 0 || style > 2 || difficulty < 0 || difficulty > 2)
        throw std::runtime_error("Invalid replay configuration");
    replay.config.seed = static_cast<std::uint32_t>(seed);
    replay.config.terrain = static_cast<TerrainStyle>(style);
    replay.config.difficulty = static_cast<Difficulty>(difficulty);
    replay.config.validate();
    auto countLine = std::istringstream(line(input));
    int count = 0;
    countLine >> count;
    endOfRecord(countLine);
    if (count < 0 || count > MaxRoundShots * replay.config.rounds) throw std::runtime_error("Invalid replay shot count");
    for (int i = 0; i < count; ++i) {
        RecordedShot shot{};
        int weapon = 0;
        auto record = std::istringstream(line(input));
        record >> shot.round >> shot.command.player >> shot.command.angle >> shot.command.power >> weapon >> shot.resultingHash;
        endOfRecord(record);
        if (shot.round < 1 || shot.round > replay.config.rounds || shot.command.player < 0 ||
            shot.command.player >= replay.config.players || weapon < 0 || weapon > 4 ||
            !std::isfinite(shot.command.angle) || shot.command.angle < 5 || shot.command.angle > 175 ||
            !std::isfinite(shot.command.power) || shot.command.power < 5 || shot.command.power > 100)
            throw std::runtime_error("Invalid replay shot");
        shot.command.weapon = static_cast<Weapon>(weapon);
        replay.shots.push_back(shot);
    }
    input >> std::ws;
    if (!input.eof()) throw std::runtime_error("Trailing replay data");
    return replay;
}

void saveReplay(const std::string& path, const Replay& replay) {
    // Write beside the destination so a failed write never truncates a prior replay.
    const auto temporary = path + ".tmp";
    try {
        std::ofstream output(temporary, std::ios::trunc);
        if (!output) throw std::runtime_error("Cannot open replay for writing: " + temporary);
        writeReplay(output, replay);
        output.close();
        if (!output) throw std::runtime_error("Could not finish replay file");
        std::filesystem::rename(temporary, path);
    } catch (...) {
        std::error_code error;
        std::filesystem::remove(temporary, error);
        throw;
    }
}

Replay loadReplay(const std::string& path) {
    std::ifstream input(path);
    if (!input) throw std::runtime_error("Cannot open replay: " + path);
    return readReplay(input);
}

ReplayPlayer::ReplayPlayer(Replay replay) : replay_(std::move(replay)), simulation_(replay_.config) {
    finished_ = replay_.shots.empty();
}

void ReplayPlayer::tick() {
    if (finished_) return;
    const auto& shot = replay_.shots[next_];
    if (!inFlight_) {
        if (simulation_.phase() == Phase::RoundOver) simulation_.nextRound();
        if (simulation_.round() != shot.round || !simulation_.fire(shot.command))
            throw std::runtime_error("Illegal replay command at shot " + std::to_string(next_ + 1));
        inFlight_ = true;
    }
    simulation_.tick();
    if (!simulation_.busy()) {
        if (simulation_.hash() != shot.resultingHash)
            throw std::runtime_error("Replay state mismatch at shot " + std::to_string(next_ + 1));
        inFlight_ = false;
        ++next_;
        finished_ = next_ == replay_.shots.size();
    }
}

Simulation verifyReplay(const Replay& replay) {
    ReplayPlayer player(replay);
    const auto budget = (replay.shots.size() + 1) * static_cast<std::size_t>(MaxShotTicks);
    for (std::size_t i = 0; i < budget && !player.finished(); ++i) player.tick();
    if (!player.finished()) throw std::runtime_error("Replay exceeded simulation tick budget");
    return player.simulation();
}

} // namespace duel
