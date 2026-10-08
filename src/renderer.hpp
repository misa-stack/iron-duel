#pragma once
#include "duel/core.hpp"
#include <SDL.h>
#include <array>
#include <memory>

namespace duel {
struct Color { Uint8 r, g, b, a = 255; };
constexpr Color Ink{230, 238, 239}, Muted{159, 184, 191}, Accent{132, 224, 173};
Color playerColor(int player);

class Renderer {
public:
    Renderer();
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    void begin();
    void battlefield(const Simulation& simulation, const ShotCommand* aim = nullptr);
    void text(int x, int y, const std::string& value, int height = 18, Color color = Ink);
    int textWidth(const std::string& value, int height = 18) const;
    void box(int x, int y, int w, int h, Color color);
    void line(int x1, int y1, int x2, int y2, Color color);
    void circle(int x, int y, int radius, Color color);
    void present();
    void screenshot(const std::string& path);
    SDL_Window* window() const { return window_.get(); }
    SDL_Renderer* raw() const { return renderer_.get(); }
private:
    struct Session { Session(); ~Session(); } session_;
    using Texture = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window_{nullptr, SDL_DestroyWindow};
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer_{nullptr, SDL_DestroyRenderer};
    Texture font_{nullptr, SDL_DestroyTexture}, ground_{nullptr, SDL_DestroyTexture}, sky_{nullptr, SDL_DestroyTexture};
    std::array<SDL_Rect, 128> glyphs_{};
    std::array<std::uint64_t, WorldWidth> revisions_{};
    std::vector<Uint32> pixels_;
    std::uint64_t terrainKey_ = ~std::uint64_t{0};
    int glyphHeight_ = 1;
    void loadFont();
    void terrain(const Terrain& terrain, std::uint64_t key);
};
}
