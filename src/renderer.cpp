#include "renderer.hpp"
#include <png.h>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <stdexcept>

namespace duel {
namespace {
void sdlCheck(int result) { if (result < 0) throw std::runtime_error(SDL_GetError()); }
Uint32 rgba(Color color) { return (static_cast<Uint32>(color.r) << 24) | (static_cast<Uint32>(color.g) << 16) |
    (static_cast<Uint32>(color.b) << 8) | color.a; }
}
Color playerColor(int player) {
    constexpr std::array<Color, 9> colors{{{106, 225, 170}, {248, 141, 115}, {132, 181, 255},
        {244, 208, 116}, {206, 161, 248}, {99, 218, 232}, {250, 160, 212}, {213, 226, 235}, {203, 190, 141}}};
    return colors.at(static_cast<std::size_t>(player));
}

Renderer::Session::Session() { sdlCheck(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)); }
Renderer::Session::~Session() { SDL_Quit(); }
Renderer::~Renderer() = default;
Renderer::Renderer() : pixels_(WorldWidth * WorldHeight) {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    window_.reset(SDL_CreateWindow("Ocelovy Duel", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   WorldWidth, WorldHeight, SDL_WINDOW_RESIZABLE));
    if (!window_) throw std::runtime_error(SDL_GetError());
    renderer_.reset(SDL_CreateRenderer(window_.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
    if (!renderer_) renderer_.reset(SDL_CreateRenderer(window_.get(), -1, SDL_RENDERER_SOFTWARE));
    if (!renderer_) throw std::runtime_error(SDL_GetError());
    sdlCheck(SDL_RenderSetLogicalSize(renderer_.get(), WorldWidth, WorldHeight));
    sdlCheck(SDL_SetRenderDrawBlendMode(renderer_.get(), SDL_BLENDMODE_BLEND));
    loadFont();
    ground_.reset(SDL_CreateTexture(renderer_.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, WorldWidth, WorldHeight));
    sky_.reset(SDL_CreateTexture(renderer_.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, WorldWidth, WorldHeight));
    if (!ground_ || !sky_) throw std::runtime_error(SDL_GetError());
    sdlCheck(SDL_SetTextureBlendMode(ground_.get(), SDL_BLENDMODE_BLEND));
    for (int y = 0; y < WorldHeight; ++y) {
        for (int x = 0; x < WorldWidth; ++x) {
            const Color color{static_cast<Uint8>(28 + y / 8), static_cast<Uint8>(60 + y / 6), static_cast<Uint8>(82 + y / 6)};
            pixels_[static_cast<std::size_t>(y * WorldWidth + x)] = rgba(color);
        }
    }
    sdlCheck(SDL_UpdateTexture(sky_.get(), nullptr, pixels_.data(), WorldWidth * 4));
    std::fill(pixels_.begin(), pixels_.end(), 0);
}

void Renderer::loadFont() {
    std::unique_ptr<char, decltype(&SDL_free)> base(SDL_GetBasePath(), SDL_free);
    const std::string name = "cislapismenamalaivelka.png";
    std::filesystem::path path = base ? std::filesystem::path(base.get()) / name : std::filesystem::path(name);
    if (!std::filesystem::exists(path)) path = std::filesystem::path(DUEL_ASSET_DIR) / name;
    png_image image{};
    image.version = PNG_IMAGE_VERSION;
    if (!png_image_begin_read_from_file(&image, path.string().c_str())) throw std::runtime_error("Cannot load font: " + path.string());
    image.format = PNG_FORMAT_RGBA;
    if (image.width > 4096 || image.height > 256) { png_image_free(&image); throw std::runtime_error("Invalid font dimensions"); }
    std::vector<Uint8> data(PNG_IMAGE_SIZE(image));
    if (!png_image_finish_read(&image, nullptr, data.data(), 0, nullptr)) {
        const std::string error = image.message; png_image_free(&image); throw std::runtime_error(error);
    }
    const int width = static_cast<int>(image.width), height = static_cast<int>(image.height);
    png_image_free(&image);
    int top = height, bottom = 0;
    for (int y = 0; y < height; ++y) for (int x = 0; x < width; ++x) {
        const auto index = static_cast<std::size_t>((y * width + x) * 4);
        if (data[index + 3]) { top = std::min(top, y); bottom = std::max(bottom, y); }
        data[index] = data[index + 1] = data[index + 2] = 255; // tint the original glyph shapes
    }
    glyphHeight_ = bottom - top + 1;
    // The original atlas contains 1-9 but no zero. Reuse O for zero instead of
    // shifting every numeric label by one as the prototype's atlas map did.
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456789";
    std::size_t character = 0;
    int start = -1;
    for (int x = 0; x <= width; ++x) {
        bool occupied = false;
        if (x < width) for (int y = 0; y < height; ++y)
            if (data[static_cast<std::size_t>((y * width + x) * 4 + 3)]) { occupied = true; break; }
        if (occupied && start < 0) start = x;
        if (!occupied && start >= 0) {
            if (character >= characters.size()) throw std::runtime_error("Unexpected font glyph count");
            glyphs_[static_cast<unsigned char>(characters[character++])] = {start, top, x - start, glyphHeight_};
            start = -1;
        }
    }
    if (character != characters.size()) throw std::runtime_error("Incomplete font atlas");
    glyphs_['0'] = glyphs_['O'];
    // Two more errors in the source atlas: Q repeats W and S repeats D.
    glyphs_['Q'] = glyphs_['O'];
    glyphs_['S'] = glyphs_['s'];
    int sTop = height, sBottom = 0;
    for (int y = 0; y < height; ++y) for (int x = glyphs_['s'].x; x < glyphs_['s'].x + glyphs_['s'].w; ++x)
        if (data[static_cast<std::size_t>((y * width + x) * 4 + 3)]) { sTop = std::min(sTop, y); sBottom = std::max(sBottom, y); }
    glyphs_['S'].y = sTop; glyphs_['S'].h = sBottom - sTop + 1;
    font_.reset(SDL_CreateTexture(renderer_.get(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, width, height));
    if (!font_) throw std::runtime_error(SDL_GetError());
    sdlCheck(SDL_UpdateTexture(font_.get(), nullptr, data.data(), width * 4));
    sdlCheck(SDL_SetTextureBlendMode(font_.get(), SDL_BLENDMODE_BLEND));
}

int Renderer::textWidth(const std::string& value, int height) const {
    int width = 0;
    for (unsigned char c : value) width += c < 128 && glyphs_[c].w ? glyphs_[c].w * height / glyphHeight_ + 2 : height / 2;
    return width;
}
void Renderer::text(int x, int y, const std::string& value, int height, Color color) {
    SDL_SetTextureColorMod(font_.get(), color.r, color.g, color.b);
    for (unsigned char c : value) {
        if (c < 128 && glyphs_[c].w) {
            SDL_Rect destination{x, y, glyphs_[c].w * height / glyphHeight_, height};
            SDL_RenderCopy(renderer_.get(), font_.get(), &glyphs_[c], &destination);
            if (c == 'Q') line(x + destination.w * 2 / 3, y + height * 2 / 3, x + destination.w, y + height, color);
            x += destination.w + 2;
        } else {
            const int w = height / 2;
            if (c == '-' || c == '+') line(x + 1, y + height / 2, x + w - 1, y + height / 2, color);
            if (c == '+') line(x + w / 2, y + height / 3, x + w / 2, y + height * 2 / 3, color);
            if (c == ':' || c == '.') box(x + w / 2, y + height * 3 / 4, 2, 2, color);
            if (c == ':') box(x + w / 2, y + height / 3, 2, 2, color);
            if (c == '/') line(x + 1, y + height, x + w - 1, y, color);
            x += w;
        }
    }
}
void Renderer::box(int x, int y, int w, int h, Color color) {
    SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
    const SDL_Rect rect{x, y, w, h}; SDL_RenderFillRect(renderer_.get(), &rect);
}
void Renderer::line(int x1, int y1, int x2, int y2, Color color) {
    SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(renderer_.get(), x1, y1, x2, y2);
}
void Renderer::circle(int x, int y, int radius, Color color) {
    for (int dy = -radius; dy <= radius; ++dy) {
        const int dx = static_cast<int>(std::sqrt(radius * radius - dy * dy));
        line(x - dx, y + dy, x + dx, y + dy, color);
    }
}
void Renderer::begin() {
    SDL_SetRenderDrawColor(renderer_.get(), 16, 29, 37, 255);
    SDL_RenderClear(renderer_.get());
    SDL_RenderCopy(renderer_.get(), sky_.get(), nullptr, nullptr);
    circle(875, 173, 39, {239, 222, 165});
}
void Renderer::terrain(const Terrain& ground, std::uint64_t key) {
    if (terrainKey_ != key) { revisions_.fill(0); terrainKey_ = key; }
    int left = WorldWidth, right = -1;
    for (int x = 0; x < WorldWidth; ++x) {
        const auto column = static_cast<std::size_t>(x);
        if (revisions_[column] == ground.revision(x)) continue;
        revisions_[column] = ground.revision(x);
        left = std::min(left, x); right = x;
        for (int y = 0; y < WorldHeight; ++y) {
            Uint32 pixel = 0;
            if (ground.solid(x, y)) {
                const int noise = (x * 17 + y * 23) % 11;
                Color color{static_cast<Uint8>(76 + noise), static_cast<Uint8>(103 + noise), static_cast<Uint8>(58 + noise)};
                if (ground.style() == TerrainStyle::Mountains) color = {static_cast<Uint8>(88 + noise), static_cast<Uint8>(102 + noise), static_cast<Uint8>(109 + noise)};
                if (ground.style() == TerrainStyle::Desert) color = {static_cast<Uint8>(171 + noise), static_cast<Uint8>(141 + noise), static_cast<Uint8>(86 + noise)};
                if (y < ground.surface(x) + 3) { color.r = static_cast<Uint8>(color.r + 30); color.g = static_cast<Uint8>(color.g + 35); color.b = static_cast<Uint8>(color.b + 20); }
                pixel = rgba(color);
            }
            pixels_[static_cast<std::size_t>(y * WorldWidth + x)] = pixel;
        }
    }
    if (right >= left) {
        const SDL_Rect rect{left, 0, right - left + 1, WorldHeight};
        sdlCheck(SDL_UpdateTexture(ground_.get(), &rect, pixels_.data() + left, WorldWidth * 4));
    }
    SDL_RenderCopy(renderer_.get(), ground_.get(), nullptr, nullptr);
}
void Renderer::battlefield(const Simulation& simulation, const ShotCommand* aim) {
    terrain(simulation.terrain(), (static_cast<std::uint64_t>(simulation.config().seed) << 16) |
        (static_cast<std::uint64_t>(simulation.round()) << 8) | static_cast<unsigned>(simulation.terrain().style()));
    for (const auto& tank : simulation.tanks()) {
        if (!tank.alive()) continue;
        const int x = static_cast<int>(tank.x), y = static_cast<int>(tank.y);
        const auto color = playerColor(tank.id);
        box(x - 22, y - 5, 44, 10, {27, 38, 41});
        circle(x, y - 10, 17, color);
        box(x - 20, y - 8, 40, 9, color);
        const double angle = aim && aim->player == tank.id ? aim->angle : tank.angle;
        const double radians = angle * 3.14159265358979323846 / 180;
        for (int d = -1; d <= 1; ++d) line(x, y - 12 + d, static_cast<int>(x + 32 * std::cos(radians)),
                                          static_cast<int>(y - 12 - 32 * std::sin(radians)) + d, color);
        box(x - 22, y - 45, 44, 4, {31, 44, 45});
        box(x - 22, y - 45, 44 * tank.health / 100, 4, color);
        const std::string name = "P" + std::to_string(tank.id + 1);
        text(x - textWidth(name, 13) / 2, y - 66, name, 13, color);
        if (tank.id == simulation.activePlayer() && simulation.phase() == Phase::Aiming)
            line(x - 8, y - 74, x + 8, y - 74, Ink);
    }
    for (const auto& projectile : simulation.projectiles()) {
        const int x = static_cast<int>(projectile.position.x), y = static_cast<int>(projectile.position.y);
        line(x, y, static_cast<int>(x - projectile.velocity.x * 0.025), static_cast<int>(y - projectile.velocity.y * 0.025), {190, 211, 184});
        circle(x, y, projectile.weapon == Weapon::Cluster ? 5 : 3, {252, 237, 170});
    }
    for (const auto& blast : simulation.explosions()) {
        const double progress = 1 - blast.ticksLeft / 24.0;
        const int radius = std::max(1, static_cast<int>(blast.radius * std::sin(progress * 3.14159265358979323846)));
        circle(static_cast<int>(blast.position.x), static_cast<int>(blast.position.y), radius, {246, 141, 70, 180});
        circle(static_cast<int>(blast.position.x), static_cast<int>(blast.position.y), radius / 2, {255, 225, 129, 220});
    }
}
void Renderer::present() { SDL_RenderPresent(renderer_.get()); }
void Renderer::screenshot(const std::string& path) {
    int width = 0, height = 0;
    sdlCheck(SDL_GetRendererOutputSize(renderer_.get(), &width, &height));
    std::vector<Uint8> data(static_cast<std::size_t>(width * height * 4));
    sdlCheck(SDL_RenderReadPixels(renderer_.get(), nullptr, SDL_PIXELFORMAT_RGBA32, data.data(), width * 4));
    png_image image{}; image.version = PNG_IMAGE_VERSION;
    image.width = static_cast<png_uint_32>(width); image.height = static_cast<png_uint_32>(height); image.format = PNG_FORMAT_RGBA;
    if (!png_image_write_to_file(&image, path.c_str(), 0, data.data(), 0, nullptr)) {
        const std::string error = image.message; png_image_free(&image); throw std::runtime_error(error);
    }
    png_image_free(&image);
}
}
