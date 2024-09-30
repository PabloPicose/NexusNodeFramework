#include <glad/glad.h>

#include "Graphics/MainWindow.h"

#include "Core/Application.h"

using namespace IRC;
int main() {
    Application app;

    std::cout << app.getFullExecutablePath() << std::endl;
    //return 0;
    MainWindow w;
    w.show();

    do {

        app.loopOnce();
    } while (!w.shouldClose());

    return 0;
}
