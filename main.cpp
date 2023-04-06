#include <cstdlib>
#include <iostream>
#include <chrono>

#include <SDL2/SDL.h>

#include "game-of-life.hpp"

constexpr int WIDTH = 1024;
constexpr int HEIGHT = 768;
constexpr int SCALE = 3;

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0)
    {
        std::cerr << "error initializing SDL: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }
    SDL_Window *win = SDL_CreateWindow("Automata",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       WIDTH, HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_Surface *surface = SDL_CreateRGBSurface(
        0, WIDTH / SCALE, HEIGHT / SCALE, 32,
        0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    SDL_FillRect(surface, NULL, 0x00000000);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
    bool do_close = false;
    bool paused = false;
    games::game_of_life game(WIDTH / SCALE, HEIGHT / SCALE);
    game.populate();
    auto clock = std::chrono::high_resolution_clock();
    auto t0 = clock.now();
    long long num_frames = 0;
    while (!do_close)
    {
        if (!paused)
        {
            if (SDL_MUSTLOCK(surface))
            {
                SDL_LockSurface(surface);
            }
            game.iterate(reinterpret_cast<uint32_t *>(surface->pixels));
            if (SDL_MUSTLOCK(surface))
            {
                SDL_UnlockSurface(surface);
            }
            SDL_UpdateTexture(texture, nullptr, surface->pixels, surface->pitch);
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, nullptr, nullptr);
            SDL_RenderPresent(renderer);
        }

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                // handling of close button
                do_close = true;
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    game.populate();
                    break;
                case SDLK_s:
                {
                    if (event.key.keysym.mod == KMOD_LCTRL)
                    {
                        std::cout << "Saving screenshot ..." << std::endl;
                        int w, h;
                        SDL_GetRendererOutputSize(renderer, &w, &h);
                        SDL_Surface *sshot = SDL_CreateRGBSurface(0, w, h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
                        SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
                        SDL_SaveBMP(sshot, "screenshot.bmp");
                        SDL_FreeSurface(sshot);
                    }
                }
                break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
        }
        ++num_frames;
        if (num_frames % 10 == 0)
        {
            auto t1 = clock.now();
            auto dt = t1 - t0;
            auto fps = 10LL * 1'000'000'000LL / dt.count();
            t0 = t1;
            std::cout << '\r' << fps << " fps\u001b[K" << std::flush;
        }
    }

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return EXIT_SUCCESS;
}