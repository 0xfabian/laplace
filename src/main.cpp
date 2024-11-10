#include <iostream>
#include <core.h>
#include <app.h>

using namespace std;
using namespace glm;

int main(int argc, char** argv)
{
    App app;

    core::init(1200, 800, "lap");
    app.init(argc > 1 ? argv[1] : nullptr);

    while (window.is_open)
    {
        core::main_loop();
        app.update(delta_time);
        app.draw();
        core::render();
    }

    app.clean();
    core::clean();

    return 0;
}