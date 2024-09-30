#ifndef IRC_MAIN_WINDOW
#define IRC_MAIN_WINDOW

#include "Core/Node.h"

class GLFWwindow;

namespace IRC {

    class MainWindow : public Node {
    public:
        explicit MainWindow(Node* parent = nullptr);

        void show();

        void update() override;

        ~MainWindow() override;

        void clear();

        void processInput();

        void swapBuffers();

        int getLastKeyPressed() const;

        bool shouldClose() const;

    private:
        void __processInput(GLFWwindow* window);

    private:
        bool m_shown = false;
        int m_width = 800;
        int m_height = 600;
        GLFWwindow* m_window = nullptr;
        int m_lastKeyPressed = -1;
    };
} // namespace IRC

#endif // !#ifndef IRC_MAIN_WINDOW
