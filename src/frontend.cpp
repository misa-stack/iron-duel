#include "duel/ai.hpp"
#include "duel/app.hpp"
#include "duel/replay.hpp"
#include "renderer.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <functional>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace duel {
namespace {
enum class Screen { Setup, Game, Pause, Controls };
struct Button { SDL_Rect rect; std::function<void()> action; };
struct ScheduledKey { int frame; SDL_Keycode key; std::string action; };
class App {
public:
    explicit App(Options options) : options_(std::move(options)), settings_(options_.config), preview_(settings_), recording_{settings_, {}} {
        if (!options_.replayPath.empty()) { playback_ = std::make_unique<ReplayPlayer>(loadReplay(options_.replayPath)); screen_ = Screen::Game; }
        else if (options_.smokeFrames && options_.smokeInputPath.empty()) start();
        if (!options_.smokeInputPath.empty()) {
            std::ifstream input(options_.smokeInputPath);
            if (!input) throw std::runtime_error("Cannot open smoke input script");
            std::string row;
            while (std::getline(input, row)) {
                if (row.empty()) continue;
                std::istringstream fields(row);
                int frame = 0; std::string action, name, extra;
                if (!(fields >> frame >> action >> name) || (fields >> extra) || frame < 0 || frame >= options_.smokeFrames ||
                    (action != "press" && action != "down" && action != "up") || scheduled_.size() >= 10000)
                    throw std::runtime_error("Invalid smoke input line: " + row);
                const auto key = SDL_GetKeyFromName(name.c_str());
                if (key == SDLK_UNKNOWN || (!scheduled_.empty() && frame < scheduled_.back().frame))
                    throw std::runtime_error("Invalid smoke key or frame order");
                scheduled_.push_back({frame, key, action});
            }
        }
        if (!options_.captureDirectory.empty()) std::filesystem::create_directories(options_.captureDirectory);
    }
    int run() {
        auto previous = std::chrono::steady_clock::now();
        int frame = 0;
        draw(); // make the first menu's buttons available before processing events
        while (running_) {
            const auto now = std::chrono::steady_clock::now();
            const double elapsed = options_.smokeFrames ? FixedDt : std::min(0.25, std::chrono::duration<double>(now - previous).count());
            previous = now;
            injectKeys(frame);
            events();
            if (screen_ == Screen::Game) update(elapsed); else clock_.reset();
            draw();
            if (!options_.captureDirectory.empty() && frame % 3 == 0) {
                std::ostringstream name;
                name << options_.captureDirectory << "/frame-" << std::setw(5) << std::setfill('0') << frame / 3 << ".png";
                renderer_.screenshot(name.str());
            }
            ++frame;
            if (options_.smokeFrames && frame >= options_.smokeFrames) {
                if (!options_.screenshotPath.empty()) renderer_.screenshot(options_.screenshotPath);
                running_ = false;
            }
            renderer_.present();
            if (!options_.smokeFrames) SDL_Delay(1);
        }
        if (!playback_ && game_ && (!options_.smokeFrames || !options_.recordPath.empty())) save();
        if (options_.smokeFrames) std::cout << "GUI smoke frames=" << frame << " completed_shots=" <<
            (playback_ ? playback_->completedShots() : recording_.shots.size()) << " screen=" << static_cast<int>(screen_)
            << " players=" << settings_.players << " humans=" << settings_.humans << " phase=" << static_cast<int>(view().phase()) << '\n';
        return replayFailed_ ? 1 : 0;
    }
private:
    Options options_;
    Config settings_;
    Simulation preview_;
    Renderer renderer_;
    std::unique_ptr<Simulation> game_;
    std::unique_ptr<ReplayPlayer> playback_;
    std::unique_ptr<AiSearch> search_;
    Replay recording_;
    std::optional<ShotCommand> pending_;
    ShotCommand aim_;
    std::vector<Trajectory> paths_;
    AiDecision lastDecision_;
    FixedClock clock_;
    Screen screen_ = Screen::Setup, helpReturn_ = Screen::Setup;
    bool running_ = true, debug_ = false, replayFailed_ = false;
    int selectedButton_ = 0, endFrames_ = 0;
    std::vector<Button> buttons_;
    std::vector<ScheduledKey> scheduled_;
    std::size_t scheduledIndex_ = 0;
    std::array<bool, 4> arrows_{}; // left, right, up, down
    std::string notice_;
    double noticeLeft_ = 0;

    const Simulation& view() const { return playback_ ? playback_->simulation() : (game_ ? *game_ : preview_); }
    void announce(std::string message) { notice_ = std::move(message); noticeLeft_ = 5; }
    void synchronizeAim() {
        if (!game_) return;
        const auto& tank = game_->tanks()[static_cast<std::size_t>(game_->activePlayer())];
        aim_ = {tank.id, tank.angle, tank.power, Weapon::Shell};
    }
    void start() {
        game_ = std::make_unique<Simulation>(settings_);
        playback_.reset(); search_.reset(); paths_.clear(); pending_.reset();
        recording_ = {settings_, {}}; screen_ = Screen::Game; endFrames_ = 0; replayFailed_ = false;
        clock_.reset(); synchronizeAim(); notice_.clear();
    }
    void setup() {
        if (game_ && !playback_) save();
        game_.reset(); playback_.reset(); search_.reset(); pending_.reset();
        preview_ = Simulation(settings_); screen_ = Screen::Setup; selectedButton_ = 0;
    }
    void save() {
        if (playback_ || !game_) return;
        const auto path = options_.recordPath.empty() ? "last-match.odr" : options_.recordPath;
        try { saveReplay(path, recording_); announce("Saved " + std::to_string(recording_.shots.size()) + " completed shots"); }
        catch (const std::exception& error) { announce(error.what()); std::cerr << error.what() << '\n'; }
    }
    void fire(const ShotCommand& command) { if (game_ && game_->fire(command)) { pending_ = command; search_.reset(); } }
    void nextRound() {
        if (game_ && game_->nextRound()) { synchronizeAim(); paths_.clear(); search_.reset(); endFrames_ = 0; clock_.reset(); }
    }
    void injectKeys(int frame) {
        while (scheduledIndex_ < scheduled_.size() && scheduled_[scheduledIndex_].frame == frame) {
            const auto& item = scheduled_[scheduledIndex_++];
            SDL_Event event{};
            event.type = item.action == "up" ? SDL_KEYUP : SDL_KEYDOWN;
            event.key.keysym.sym = item.key;
            event.key.keysym.scancode = SDL_GetScancodeFromKey(item.key);
            if (SDL_PushEvent(&event) < 0) throw std::runtime_error(SDL_GetError());
            if (item.action == "press") { event.type = SDL_KEYUP; SDL_PushEvent(&event); }
        }
    }
    void events() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) { running_ = false; continue; }
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                arrows_.fill(false);
                if (screen_ == Screen::Game && !options_.smokeFrames) screen_ = Screen::Pause;
            }
            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                const std::array<SDL_Keycode, 4> keys{{SDLK_LEFT, SDLK_RIGHT, SDLK_UP, SDLK_DOWN}};
                for (std::size_t i = 0; i < keys.size(); ++i) if (event.key.keysym.sym == keys[i]) arrows_[i] = event.type == SDL_KEYDOWN;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                float x = 0, y = 0;
                SDL_RenderWindowToLogical(renderer_.raw(), event.button.x, event.button.y, &x, &y);
                for (const auto& button : buttons_) if (x >= button.rect.x && x < button.rect.x + button.rect.w && y >= button.rect.y && y < button.rect.y + button.rect.h) {
                    const auto before = screen_;
                    auto action = button.action; action();
                    if (screen_ != before) buttons_.clear();
                    break;
                }
            }
            if (event.type != SDL_KEYDOWN || event.key.repeat) continue;
            const auto key = event.key.keysym.sym;
            if (key == SDLK_F11) {
                const bool full = (SDL_GetWindowFlags(renderer_.window()) & SDL_WINDOW_FULLSCREEN_DESKTOP) != 0;
                if (SDL_SetWindowFullscreen(renderer_.window(), full ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP) < 0) announce(SDL_GetError());
                continue;
            }
            if (key == SDLK_ESCAPE) {
                if (screen_ == Screen::Controls) screen_ = helpReturn_;
                else if (screen_ == Screen::Pause) screen_ = Screen::Game;
                else if (screen_ == Screen::Game) { screen_ = Screen::Pause; selectedButton_ = 0; }
                else running_ = false;
                buttons_.clear();
                continue;
            }
            if (screen_ != Screen::Game) {
                if (!buttons_.empty() && (key == SDLK_UP || key == SDLK_DOWN))
                    selectedButton_ = (selectedButton_ + (key == SDLK_UP ? -1 : 1) + static_cast<int>(buttons_.size())) % static_cast<int>(buttons_.size());
                if (key == SDLK_RETURN && !buttons_.empty()) {
                    const auto before = screen_;
                    auto action = buttons_[std::min(static_cast<std::size_t>(selectedButton_), buttons_.size() - 1)].action; action();
                    if (screen_ != before) buttons_.clear();
                }
                continue;
            }
            if (key == SDLK_F1) debug_ = !debug_;
            if (key == SDLK_F5) save();
            if (playback_) { if (key == SDLK_SPACE) screen_ = Screen::Pause; continue; }
            if (!game_) continue;
            if (key == SDLK_RETURN && game_->phase() == Phase::RoundOver) nextRound();
            else if (key == SDLK_RETURN && game_->phase() == Phase::MatchOver) setup();
            if (game_ && game_->phase() == Phase::Aiming && game_->tanks()[static_cast<std::size_t>(game_->activePlayer())].human) {
                if (key >= SDLK_1 && key <= SDLK_5) aim_.weapon = static_cast<Weapon>(key - SDLK_1);
                if (key == SDLK_SPACE) fire(aim_);
            }
        }
    }
    void update(double elapsed) {
        noticeLeft_ = std::max(0.0, noticeLeft_ - elapsed);
        if (playback_) {
            if (replayFailed_) return;
            try { for (int i = clock_.advance(elapsed); i > 0; --i) playback_->tick(); }
            catch (const std::exception& error) { replayFailed_ = true; announce(error.what()); std::cerr << error.what() << '\n'; }
            return;
        }
        if (!game_) return;
        if (game_->phase() == Phase::Aiming) {
            const auto& tank = game_->tanks()[static_cast<std::size_t>(game_->activePlayer())];
            if (tank.human) {
                aim_.angle = std::clamp(aim_.angle + (arrows_[0] - arrows_[1]) * elapsed * 40, 5.0, 175.0);
                aim_.power = std::clamp(aim_.power + (arrows_[2] - arrows_[3]) * elapsed * 30, 5.0, 100.0);
            } else {
                if (!search_) search_ = std::make_unique<AiSearch>(*game_);
                if (search_->advance()) {
                    lastDecision_ = search_->decision(); paths_ = search_->preview(); aim_ = lastDecision_.command; fire(aim_);
                }
            }
        }
        for (int i = clock_.advance(elapsed); i > 0; --i) game_->tick();
        if (pending_ && !game_->busy()) {
            recording_.shots.push_back({game_->round(), *pending_, game_->hash()});
            pending_.reset(); synchronizeAim();
            if ((!options_.recordPath.empty() || game_->phase() == Phase::MatchOver) && (!options_.smokeFrames || !options_.recordPath.empty())) save();
        }
        if (game_->phase() == Phase::RoundOver || game_->phase() == Phase::MatchOver) {
            ++endFrames_;
            if (options_.smokeFrames && endFrames_ > 100 && game_->phase() == Phase::RoundOver) nextRound();
        }
    }
    void button(int x, int y, int width, const std::string& label, std::function<void()> action) {
        int mx = 0, my = 0; float lx = 0, ly = 0;
        SDL_GetMouseState(&mx, &my); SDL_RenderWindowToLogical(renderer_.raw(), mx, my, &lx, &ly);
        const bool hover = lx >= x && lx < x + width && ly >= y && ly < y + 34;
        const bool selected = static_cast<int>(buttons_.size()) == selectedButton_;
        renderer_.box(x, y, width, 34, hover || selected ? Color{54, 89, 92} : Color{31, 52, 62});
        if (selected) renderer_.box(x, y, 3, 34, Accent);
        renderer_.text(x + 14, y + 8, label, 17, hover || selected ? Ink : Muted);
        buttons_.push_back({{x, y, width, 34}, std::move(action)});
    }
    void panel(int x, int y, int width, int height) {
        renderer_.box(x, y, width, height, {14, 29, 39, 243}); renderer_.box(x, y, width, 2, {111, 155, 154});
    }
    void drawSetup() {
        renderer_.battlefield(preview_); panel(38, 32, 463, 540);
        renderer_.text(61, 57, "OCELOVY DUEL", 30);
        renderer_.text(61, 101, "Angles. Steel. Shifting ground.", 15, Muted);
        button(60, 140, 418, "Start match", [&] { start(); });
        button(60, 181, 418, "Players  " + std::to_string(settings_.players), [&] {
            settings_.players = settings_.players == 9 ? 2 : settings_.players + 1;
            settings_.humans = std::min(settings_.humans, settings_.players); preview_ = Simulation(settings_);
        });
        button(60, 222, 418, "Human players  " + std::to_string(settings_.humans), [&] { settings_.humans = (settings_.humans + 1) % (settings_.players + 1); preview_ = Simulation(settings_); });
        button(60, 263, 418, std::string("AI difficulty  ") + difficultyName(settings_.difficulty), [&] { settings_.difficulty = static_cast<Difficulty>((static_cast<int>(settings_.difficulty) + 1) % 3); });
        button(60, 304, 418, std::string("Terrain  ") + terrainName(settings_.terrain), [&] { settings_.terrain = static_cast<TerrainStyle>((static_cast<int>(settings_.terrain) + 1) % 3); preview_ = Simulation(settings_); });
        button(60, 345, 418, "Rounds  " + std::to_string(settings_.rounds), [&] { settings_.rounds = settings_.rounds == 9 ? 1 : settings_.rounds + 1; });
        button(60, 386, 418, "New map seed", [&] { settings_.seed = Random(settings_.seed).next(); preview_ = Simulation(settings_); });
        button(60, 427, 418, "Controls", [&] { helpReturn_ = Screen::Setup; screen_ = Screen::Controls; selectedButton_ = 0; });
        button(60, 468, 418, "Quit", [&] { running_ = false; });
        renderer_.text(61, 530, "Seed " + std::to_string(settings_.seed), 14, Muted);
        panel(541, 60, 483, 264);
        renderer_.text(566, 86, "Make every shot count", 22, Accent);
        renderer_.text(566, 128, "Choose your angle and power.", 17);
        renderer_.text(566, 159, "Blast craters. Bring the hills down.", 17);
        renderer_.text(566, 190, "Last tank standing wins the round.", 17);
        renderer_.text(566, 245, "Play solo against AI or share a keyboard.", 15, Muted);
        renderer_.text(566, 274, "Human players take the first player slots.", 15, Muted);
    }
    void drawHud(const Simulation& simulation) {
        renderer_.box(0, 0, WorldWidth, 116, {13, 29, 39, 248});
        renderer_.text(20, 15, "OCELOVY DUEL", 20);
        renderer_.text(313, 18, "Round " + std::to_string(simulation.round()) + "/" + std::to_string(simulation.config().rounds), 16, Muted);
        renderer_.text(530, 18, phaseName(simulation.phase()), 16, Accent);
        renderer_.text(833, 19, "Seed " + std::to_string(simulation.config().seed), 12, Muted);
        const int cellWidth = (WorldWidth - 40) / simulation.config().players;
        for (const auto& tank : simulation.tanks()) {
            const int x = 20 + tank.id * cellWidth;
            renderer_.text(x, 49, "P" + std::to_string(tank.id + 1) + "  " + std::to_string(tank.health), 15, tank.alive() ? playerColor(tank.id) : Muted);
            renderer_.text(x + 77, 52, tank.human ? "H" : "AI", 11, Muted);
            renderer_.box(x, 72, cellWidth - 18, 3, {43, 61, 66});
            if (tank.id == simulation.activePlayer()) renderer_.box(x, 72, cellWidth - 18, 3, playerColor(tank.id));
        }
        if (!playback_ && game_ && simulation.phase() == Phase::Aiming) {
            const auto& tank = simulation.tanks()[static_cast<std::size_t>(simulation.activePlayer())];
            if (tank.human) {
                const int ammo = tank.ammo[static_cast<std::size_t>(aim_.weapon)];
                renderer_.text(20, 88, "Angle " + std::to_string(static_cast<int>(aim_.angle)) + "   Power " + std::to_string(static_cast<int>(aim_.power)) +
                    "   " + weaponSpec(aim_.weapon).name + "   Ammo " + (ammo < 0 ? "unlimited" : std::to_string(ammo)), 16, ammo == 0 ? playerColor(1) : Ink);
                const double angle = aim_.angle * 3.14159265358979323846 / 180;
                renderer_.line(static_cast<int>(tank.x), static_cast<int>(tank.y - 12), static_cast<int>(tank.x + 44 * std::cos(angle)), static_cast<int>(tank.y - 12 - 44 * std::sin(angle)), Ink);
            } else renderer_.text(20, 89, "P" + std::to_string(tank.id + 1) + " is choosing a shot", 16, Muted);
        } else renderer_.text(20, 89, playback_ ? "Replay  -  " + std::to_string(playback_->completedShots()) + " shots verified" :
            (simulation.busy() ? "Wait for the shot and terrain to settle" : "Round complete  -  scores below"), 16, Muted);
        renderer_.box(0, 574, WorldWidth, 26, {13, 29, 39, 240});
        renderer_.text(16, 581, "Arrows aim and power   1-5 weapon   Space fire   Esc pause   F1 trajectories   F5 save   F11 fullscreen", 11, Muted);
    }
    void drawResults(const Simulation& simulation) {
        const bool matchOver = simulation.phase() == Phase::MatchOver;
        std::string title;
        if (matchOver) {
            int wins = -1, winner = -1, tied = 0;
            for (const auto& tank : simulation.tanks()) {
                if (tank.wins > wins) { wins = tank.wins; winner = tank.id; tied = 1; }
                else if (tank.wins == wins) ++tied;
            }
            title = tied > 1 ? "Match tied" : "P" + std::to_string(winner + 1) + " wins the match";
        } else title = simulation.roundWinner() ? "P" + std::to_string(*simulation.roundWinner() + 1) + " wins the round" : "Round drawn";
        panel(279, 150, 510, 140 + simulation.config().players * 28);
        renderer_.text(306, 174, title, 24, Accent);
        int y = 219;
        for (const auto& tank : simulation.tanks()) {
            renderer_.text(306, y, "P" + std::to_string(tank.id + 1) + "   wins " + std::to_string(tank.wins) + "   damage " + std::to_string(tank.damageDealt), 17, playerColor(tank.id)); y += 28;
        }
        renderer_.text(306, y + 16, playback_ ? "Replay verified" : (matchOver ? "Enter for a new match" : "Enter for the next round"), 17, Muted);
    }
    void drawControls() {
        panel(152, 70, 763, 466); renderer_.text(181, 100, "Controls", 27, Accent);
        const std::array<std::string, 9> rows{{"Left and Right   Rotate the barrel", "Up and Down   Set shot power", "1-5   Shell / Rocket / Bouncer / Cluster / Ultimate",
            "Space   Fire when your turn is ready", "Enter   Continue after a round", "F1   Show AI candidate trajectories", "F5   Save completed shots to a replay", "Esc   Pause or return", "F11   Toggle fullscreen"}};
        int y = 154;
        for (const auto& row : rows) { renderer_.text(181, y, row, 17); y += 32; }
        button(181, 466, 235, "Back", [&] { screen_ = helpReturn_; selectedButton_ = 0; });
    }
    void draw() {
        buttons_.clear(); renderer_.begin();
        if (screen_ == Screen::Setup) drawSetup();
        else {
            const ShotCommand* displayedAim = !playback_ && game_ && game_->phase() == Phase::Aiming &&
                game_->tanks()[static_cast<std::size_t>(game_->activePlayer())].human ? &aim_ : nullptr;
            renderer_.battlefield(view(), displayedAim);
            if (screen_ != Screen::Controls) {
                if (debug_) {
                    const auto& paths = search_ ? search_->preview() : paths_;
                    for (std::size_t i = 0; i < paths.size(); ++i) for (const auto& point : paths[i].points)
                        if (point.y > 116 && point.y < 570) renderer_.circle(static_cast<int>(point.x), static_cast<int>(point.y), i + 1 == paths.size() ? 2 : 1, i + 1 == paths.size() ? Accent : Color{148, 182, 182, 100});
                    renderer_.box(15, 124, 420, 25, {13, 29, 39, 220});
                    renderer_.text(24, 131, search_ ? "AI search in progress" : "Last AI search  " + std::to_string(lastDecision_.candidates) + " candidates  " + std::to_string(lastDecision_.fullSimulations) + " finalists", 12, Accent);
                }
                drawHud(view());
                if (view().phase() == Phase::RoundOver || view().phase() == Phase::MatchOver) drawResults(view());
                else if (playback_ && playback_->finished()) {
                    panel(301, 194, 464, 99); renderer_.text(329, 222, "Replay complete and verified", 20, Accent); renderer_.text(329, 255, "Esc for menu", 15, Muted);
                }
            }
            if (screen_ == Screen::Controls) drawControls();
            if (screen_ == Screen::Pause) {
                panel(327, 161, 413, 292); renderer_.text(351, 183, "Paused", 25);
                button(351, 228, 365, "Resume", [&] { screen_ = Screen::Game; clock_.reset(); });
                button(351, 270, 365, "New match", [&] { setup(); });
                button(351, 312, 365, "Controls", [&] { helpReturn_ = Screen::Pause; screen_ = Screen::Controls; selectedButton_ = 0; });
                button(351, 354, 365, "Save replay", [&] { save(); });
                button(351, 396, 365, "Quit", [&] { running_ = false; });
            }
        }
        if ((noticeLeft_ > 0 || replayFailed_) && !notice_.empty()) {
            renderer_.box(14, 542, WorldWidth - 28, 28, {14, 29, 39, 245}); renderer_.text(24, 550, notice_, 13, replayFailed_ ? playerColor(1) : Accent);
        }
    }
};
}
int runFrontend(const Options& options) { return App(options).run(); }
}
