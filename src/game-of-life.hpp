#ifndef __GAME_OF_LIFE_HPP__
#define __GAME_OF_LIFE_HPP__

#include <array>
#include <cstdlib>
#include <memory>
#include <random>
#include <vector>
#include <utility>
#include <sstream>

#include "game.hpp"
#include "util.hpp"

namespace games
{
    namespace
    {
        unsigned int mod(int x, int n)
        {
            return static_cast<unsigned int>(((x % n) + n) % n);
        }
    }

    class game_of_life final : public game
    {
        static constexpr uint32_t ALIVE_COLOR = 0xfff01020;
        static constexpr uint32_t DEAD_COLOR = 0xffcc8000;

    public:
        static const std::string BEACON1;
        static const std::string TWO_GUN;
        static const std::string GOSPER_GUN;
        static const std::string SCHICK256;
        static const std::string TWO_ENGINE_CORDERSHIP;

        enum cell_state
        {
            DEAD,
            ALIVE,
        };

        game_of_life() = delete;

        game_of_life(int width, int height, uint32_t *pixels)
            : width(width), height(height), pixels(pixels)
        {
            plane_a = std::make_unique<std::vector<cell_state>>(width * height, DEAD);
            plane_b = std::make_unique<std::vector<cell_state>>(width * height, DEAD);
            neighbors = {-1 - width, -width, 1 - width, 1, 1 + width, width, width - 1, -1};
            rng.seed(static_cast<uint32_t>(util::make_seed()));
            // warmup RNG
            for (int i = 0; i < 10'000; ++i)
            {
                (void)rng();
            }
        }

        void populate()
        {
            for (unsigned int i = 0; i < static_cast<unsigned int>(width * height); ++i)
            {
                (*plane_a)[i] = ((rng() & 1) == 0) ? DEAD : ALIVE;
            }
        }

        void clear()
        {
            std::fill(plane_a->begin(), plane_a->end(), DEAD);
            std::fill(plane_b->begin(), plane_b->end(), DEAD);
        }

        void emplace(int const x, int const y, std::string const &obj)
        {
            int j = 0;
            int i = 0;
            for (char const &c : obj)
            {
                if (c == '\n')
                {
                    i = 0;
                    ++j;
                }
                else
                {
                    set(x + i, y + j, c == '.' ? DEAD : ALIVE);
                    ++i;
                }
            }
        }

        void irritate(int const x, int const y)
        {
            for (int dy = -1; dy <= 1; ++dy)
            {
                for (int dx = -1; dx <= 1; ++dx)
                {
                    set(x + dx, y + dy, (rng() & 1) == 0 ? games::game_of_life::DEAD : games::game_of_life::ALIVE);
                }
            }
        }

        void set(int x, int y, cell_state state)
        {
            (*plane_a)[mod(y, height) * static_cast<unsigned int>(width) + mod(x, width)] = state;
        }

        void iterate()
        {
            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    unsigned int const current_idx = static_cast<unsigned int>(y * width + x);
                    int num_alive = 0;
                    for (auto neighbor_idx : neighbors)
                    {
                        num_alive += plane_a->at(mod(static_cast<int>(current_idx) + neighbor_idx, width * height));
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
        uint32_t *pixels{nullptr};
        std::unique_ptr<std::vector<cell_state>> plane_a;
        std::unique_ptr<std::vector<cell_state>> plane_b;
        std::array<int, 8> neighbors;
        std::mt19937 rng;
    };
}

#endif // __GAME_OF_LIFE_HPP__
