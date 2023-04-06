#ifndef __GAME_OF_LIFE_HPP__
#define __GAME_OF_LIFE_HPP__

#include <array>
#include <cstdlib>
#include <memory>
#include <random>
#include <vector>
#include <utility>

#include "util.hpp"

namespace games
{
    int mod(int x, int n)
    {
        return ((x % n) + n) % n;
    }

    class game_of_life
    {
        enum cell_state {
            DEAD,
            ALIVE,
        };
        static constexpr uint32_t ALIVE_COLOR = 0xfff01020;
        static constexpr uint32_t DEAD_COLOR = 0xffcc8000;

    public:
        game_of_life() = delete;
        game_of_life(int width, int height)
            : width(width), height(height)
        {
            plane_a = std::make_unique<std::vector<cell_state>>(width * height, DEAD);
            plane_b = std::make_unique<std::vector<cell_state>>(width * height, DEAD);
            neighbors[0] = -1 - width;
            neighbors[1] = -width;
            neighbors[2] = 1 - width;
            neighbors[3] = 1;
            neighbors[4] = 1 + width;
            neighbors[5] = width;
            neighbors[6] = width - 1;
            neighbors[7] = -1;
            rng.seed(static_cast<uint32_t>(util::make_seed()));
            // warmup RNG
            for (int i = 0; i < 10'000; ++i)
            {
                (void)rng();
            }
        }

        void populate()
        {
            for (int i = 0; i < width * height; ++i)
            {
                (*plane_a)[i] = ((rng() & 1) == 0) ? DEAD : ALIVE;
            }
        }

        void set(int x, int y, cell_state state)
        {
            assert(x >= 0 && x < width);
            assert(y >= 0 && y < height);
            (*plane_a)[y * width + x] = state;
        }

        void iterate(uint32_t *pixels)
        {
            assert(pixels != nullptr);
            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    int const current_idx = y * width + x;
                    int num_alive = 0;
                    for (auto neighbor_idx : neighbors)
                    {
                        num_alive += plane_a->at(mod(current_idx + neighbor_idx, width * height));
                    }
                    if (plane_a->at(current_idx) == ALIVE)
                    {
                        if (num_alive == 2 || num_alive == 3)
                        {
                            (*plane_b)[current_idx] = ALIVE;
                            pixels[current_idx] = ALIVE_COLOR;
                        }
                        else
                        {
                            (*plane_b)[current_idx] = DEAD;
                            pixels[current_idx] = DEAD_COLOR;
                        }
                    }
                    else
                    {
                        if (num_alive == 3)
                        {
                            (*plane_b)[current_idx] = ALIVE;
                            pixels[current_idx] = ALIVE_COLOR;
                        }
                        else
                        {
                            (*plane_b)[current_idx] = DEAD;
                            pixels[current_idx] = ((pixels[current_idx] >> 1) & 0xff000000) | (pixels[current_idx] & 0x00ffffff);
                        }
                    }
                }
            }
            std::swap(plane_a, plane_b);
        }

    private:
        const int width;
        const int height;
        std::unique_ptr<std::vector<cell_state>> plane_a;
        std::unique_ptr<std::vector<cell_state>> plane_b;
        std::array<int, 8> neighbors;
        std::mt19937 rng;
    };
}

#endif // __GAME_OF_LIFE_HPP__
