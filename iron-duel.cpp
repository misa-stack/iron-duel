#include <SDL2/SDL.h>
#include <vector>
#include <cmath>

struct Projectile {
    float x, y, vx, vy;
    bool active;
};

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Iron Duel", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Projectile shot = {100, 500, 5, -10, false};
    bool running = true;

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) shot.active = true;
        }

        if (shot.active) {
            shot.vy += 0.15f; // Gravity
            shot.x += shot.vx;
            shot.y += shot.vy;
            if (shot.y > 600) shot.active = false; // Reset if it hits ground
        }

        SDL_SetRenderDrawColor(renderer, 5, 5, 5, 255); // Background
        SDL_RenderClear(renderer);

        // Draw Ground
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_Rect ground = {0, 550, 800, 50};
        SDL_RenderFillRect(renderer, &ground);

        // Draw Player Tank
        SDL_SetRenderDrawColor(renderer, 99, 102, 241, 255); // Indigo
        SDL_Rect player = {80, 530, 40, 20};
        SDL_RenderFillRect(renderer, &player);

        // Draw Shot
        if (shot.active) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_Rect p_rect = {(int)shot.x, (int)shot.y, 5, 5};
            SDL_RenderFillRect(renderer, &p_rect);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
