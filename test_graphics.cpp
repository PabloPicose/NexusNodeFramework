#include <glad/glad.h>

#include "GUI/GUIWindow.h"

#include "Core/Application.h"
#include "imgui.h"

using namespace IRC;
int main() {
    Application app;

    std::cout << app.getFullExecutablePath() << std::endl;
    //return 0;
    GUIWindow w;
    //w.show();

    return app.run();
}
