#ifndef __UI_UI_ELEMENT_HPP__
#define __UI_UI_ELEMENT_HPP__

#include <memory>

#include "context.hpp"

namespace ui
{
    class ui_element
    {
    public:
        ui_element() {}
        ui_element(std::shared_ptr<context> ctx) : ctx_(ctx) {}
        virtual void render() = 0;
        virtual void on_mouse_enter() {}
        virtual void on_mouse_leave() {}
        virtual void on_mouse_button_down(int) {}
        virtual void on_mouse_button_up(int) {}
        virtual void on_mouse_wheel(int) {}

        inline std::shared_ptr<context> ctx()
        {
            return ctx_;
        }

    protected:
        std::shared_ptr<context> ctx_;
    };

}
#endif // __UI_UI_ELEMENT_HPP__
