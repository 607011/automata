#ifndef __UI_LABEL_HPP__
#define __UI_LABEL_HPP__

#include <memory>
#include <string>

#include <SDL.h>

#include "ui_element.hpp"

namespace ui
{
    class label : public ui_element
    {
    public:
        label() : ui_element() {}
        label(std::shared_ptr<context> ctx)
            : ui_element(ctx)
            , color_(ctx->foreground_color())
        {
        }
        label(std::shared_ptr<context> ctx, std::string const &text)
            : ui_element(ctx), text_(text)
        {
        }
        label(std::shared_ptr<context> ctx, SDL_Rect pos, std::string const &text)
            : ui_element(ctx), pos_(pos), text_(text)
        {
        }
        label(std::shared_ptr<context> ctx, SDL_Rect pos, SDL_Color color, std::string const &text)
            : ui_element(ctx), pos_(pos), color_(color), text_(text)
        {
        }
        void render()
        {
            SDL_Surface *text_surface = TTF_RenderText_Solid(ctx()->font(), text_.c_str(), color_);
            pos_.w = text_surface->w;
            pos_.h = text_surface->h;
            SDL_Texture *font_texture = SDL_CreateTextureFromSurface(ctx()->renderer(), text_surface);
            SDL_RenderCopy(ctx()->renderer(), font_texture, nullptr, &pos_);
            SDL_DestroyTexture(font_texture);
            SDL_FreeSurface(text_surface);
        }
        void set_pos(SDL_Rect pos)
        {
            pos_ = pos;
        }
        void set_text(std::string const &text)
        {
            text_ = text;
        }
        void set_color(SDL_Color color)
        {
            color_ = color;
        }
        std::string const &text() const
        {
            return text_;
        }

    private:
        SDL_Rect pos_{0, 0, 0, 0};
        SDL_Color color_{255, 255, 255, 200};
        std::string text_;
    };

}

#endif // __UI_LABEL_HPP__
