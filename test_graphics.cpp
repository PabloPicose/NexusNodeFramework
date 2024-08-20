#include "Graphics/MainWindow.h"

#include "Core/Application.h"

using namespace IRC;
int main() {
    Application app;
    MainWindow w;
    w.show();

    return app.run();
}
