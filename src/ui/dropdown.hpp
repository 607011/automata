#ifndef __UI_DROPDOWN_HPP__
#define __UI_DROPDOWN_HPP__

#include <memory>
#include <string>

#include "ui_element.hpp"

namespace ui
{
    class dropdown : public ui_element
    {
    public:
        dropdown(std::shared_ptr<context> ctx)
            : ui_element(ctx)
        {
        }

        void render()
        {
        }
    };

}

#endif // __UI_DROPDOWN_HPP__
