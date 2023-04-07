#ifndef __APP_HPP__
#define __APP_HPP__

#include <iostream>
#include <iomanip>
#include <random>
#include <cstdlib>
#include <memory>
#include <sstream>

#include <SDL.h>
#include <SDL_ttf.h>

#include "game-of-life.hpp"
#include "ui/ui.hpp"

class app
{
    static constexpr int DEFAULT_WIDTH = 640;
    static constexpr int DEFAULT_HEIGHT = 480;
    static constexpr int DEFAULT_SCALE = 3;

public:
    app()
        : width(DEFAULT_WIDTH), height(DEFAULT_HEIGHT), scale(DEFAULT_SCALE)
    {
        ready_ = setup_ui();
        game = std::make_unique<games::game_of_life>(
            width / scale,
            height / scale,
            reinterpret_cast<uint32_t *>(surface->pixels));
    }

    virtual ~app()
    {
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(win);
        SDL_Quit();
    }

    void loop()
    {
        auto t0 = SDL_GetPerformanceCounter();
        long long num_frames = 0;
        const float freq = static_cast<float>(SDL_GetPerformanceFrequency());
        while (!do_close)
        {
            if (!paused)
            {
                render();
                if (++num_frames % 10 == 0)
                {
                    auto t1 = SDL_GetPerformanceCounter();
                    float seconds_elapsed = (t1 - t0) / freq;
                    t0 = t1;
                    std::stringstream ss;
                    ss << std::setprecision(4) << (10.f / seconds_elapsed) << " fps";
                    fps_label.set_text(ss.str());
                }
                fps_label.render();
                SDL_RenderPresent(renderer);
                iterate();
            }
            handle_events();
        }
    }

    bool is_ready() const
    {
        return ready_;
    }

private:
    void render()
    {
        SDL_UpdateTexture(texture, nullptr, surface->pixels, surface->pitch);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    }

    bool setup_ui()
    {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0)
        {
            std::cerr << "\u001b[31;1mError initializing SDL:\u001b[0m " << SDL_GetError() << std::endl;
            return false;
        }
        win = SDL_CreateWindow(
            "Automata",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width, height,
            SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL);
        if (win == nullptr)
        {
            std::cerr << "\u001b[31;1mError initializing windows:\u001b[0m " << SDL_GetError() << std::endl;
            return false;
        }
        renderer = SDL_CreateRenderer(
            win, -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC);
        if (renderer == nullptr)
        {
            std::cerr << "\u001b[31;1mError initializing renderer:\u001b[0m " << SDL_GetError() << std::endl;
            return false;
        }
        surface = SDL_CreateRGBSurface(
            0, width / scale, height / scale, 32,
            0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
        if (surface == nullptr)
        {
            std::cerr << "\u001b[31;1mError initializing surface:\u001b[0m " << SDL_GetError() << std::endl;
            return false;
        }
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture == nullptr)
        {
            std::cerr << "\u001b[31;1mError initializing texture:\u001b[0m " << SDL_GetError() << std::endl;
            return false;
        }
        SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);
        SDL_RenderSetVSync(renderer, 1);
        if (TTF_Init() < 0)
        {
            std::cerr << "\u001b[31;1mError initializing SDL_ttf:\u001b[0m " << TTF_GetError() << std::endl;
            return false;
        }
        font = TTF_OpenFont("data/VT323-Regular.ttf", 32);
        if (font == nullptr)
        {
            std::cerr << "\u001b[31;1mError loading font:\u001b[0m: " << TTF_GetError() << std::endl;
            return false;
        }

        ctx = std::make_shared<ui::context>(renderer, font);
        fps_label = ui::label(ctx, {8, 8, 0, 0}, {255, 255, 255, 200}, "? fps");

        return true;
    }

    inline void iterate()
    {
        game->iterate();
    }

    void handle_events()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == 1)
                {
                    mouse_down = true;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == 1)
                {
                    if (!mouse_moved)
                    {
                        int const x = event.motion.x / scale;
                        int const y = event.motion.y / scale;
                        game->emplace(x, y, games::game_of_life::TWOENGINESHIP);
                    }
                    mouse_moved = false;
                    mouse_down = false;
                }
                break;
            case SDL_MOUSEMOTION:
            {
                if (mouse_down)
                {
                    int const x = event.motion.x / scale;
                    int const y = event.motion.y / scale;
                    game->irritate(x, y);
                    mouse_moved = true;
                }
            }
            break;
            case SDL_QUIT:
                do_close = true;
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    game->populate();
                    break;
                case SDLK_SPACE:
                    paused = !paused;
                    break;
                case SDLK_c:
                    game->clear();
                    break;
                case SDLK_q:
                    do_close = true;
                    break;
                case SDLK_s:
                {
                    if (event.key.keysym.mod == KMOD_LCTRL)
                    {
                        std::string filename = "screenshot-" + util::iso_datetime_now() + ".bmp";
                        std::cout << "Saving screenshot to " << filename << " ..." << std::endl;
                        SDL_SaveBMP(surface, filename.c_str());
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
    }

    int width;
    int height;
    int scale;
    bool ready_{false};
    std::unique_ptr<games::game_of_life> game;

    std::shared_ptr<ui::context> ctx;
    ui::label fps_label{};

    SDL_Window *win;
    SDL_Renderer *renderer;
    SDL_Surface *surface;
    SDL_Texture *texture;
    TTF_Font *font;
    bool paused{false};
    bool do_close{false};
    bool mouse_down{false};
    bool mouse_moved{false};
};

#endif // __APP_HPP__
