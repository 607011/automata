#ifndef __UI_CONTEXT_HPP__
#define __UI_CONTEXT_HPP__

#include <SDL.h>
#include <SDL_ttf.h>

namespace ui
{
    class context
    {
    public:
        context() = delete;
        context(SDL_Renderer *renderer, TTF_Font *font)
            : renderer_(renderer)
            , font_(font)
        {
        }
        context(SDL_Renderer *renderer, TTF_Font *font, SDL_Color fg, SDL_Color bg, SDL_Color hi)
            : renderer_(renderer)
            , font_(font)
            , foreground_color_(fg)
            , background_color_(bg)
            , highlight_color_(hi)
        {
        }
        inline TTF_Font *font()
        {
            return font_;
        }
        inline SDL_Renderer *renderer()
        {
            return renderer_;
        }
        inline SDL_Color foreground_color() const
        {
            return foreground_color_;
        }
        inline SDL_Color background_color() const
        {
            return background_color_;
        }
        inline SDL_Color highlight_color() const
        {
            return background_color_;
        }

    protected:
        SDL_Renderer *renderer_{nullptr};
        TTF_Font *font_{nullptr};
        SDL_Color foreground_color_{255, 255, 255, 255};
        SDL_Color background_color_{10, 10, 10, 200};
        SDL_Color highlight_color_{191, 255, 0, 255};
    };
}

#endif // __UI_CONTEXT_HPP__
