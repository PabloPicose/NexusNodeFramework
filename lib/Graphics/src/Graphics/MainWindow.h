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

    private:
        void processInput(GLFWwindow* window);

    private:
        bool m_shown = false;
        int m_width = 800;
        int m_height = 600;
        GLFWwindow* m_window = nullptr;
    };
} // namespace IRC

#endif // !#ifndef IRC_MAIN_WINDOW
