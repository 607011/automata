#include <cstdlib>
#include <memory>

#include "app.hpp"

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
    auto a = std::make_unique<app>();
    if (a->is_ready())
    {
        a->loop();
    }
    return EXIT_SUCCESS;
}
