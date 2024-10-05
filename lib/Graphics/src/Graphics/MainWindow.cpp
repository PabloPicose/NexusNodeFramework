#include "Graphics/MainWindow.h"
#include "Core/Application.h"

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include "MainWindow.h"


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void processWindowsClose(GLFWwindow* window) {
    if (glfwWindowShouldClose(window)) {
        IRC::Application::instance()->quit();
    }
}

namespace IRC {
    MainWindow::MainWindow(Node* parent) : Node(parent) {
        // glfw: initialize and configure
        // ------------------------------
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        // glfw window creation
        // --------------------
        m_window = glfwCreateWindow(m_width, m_height, "MainWindow", NULL, NULL);
        if (m_window == nullptr) {
            std::cout << "Failed to create GLFW window" << std::endl;
        }
        glfwMakeContextCurrent(m_window);
        glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
        glfwSetWindowCloseCallback(m_window, processWindowsClose);

        // glad: load all OpenGL function pointers
        // ---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
        }
    }

    void MainWindow::update() {
        if (!m_shown) {
            return;
        }
        processInput();
        clear();
        // render ---


        swapBuffers();
    }
    void MainWindow::show() { m_shown = true; }

    MainWindow::~MainWindow() { glfwTerminate(); }

    void MainWindow::__processInput(GLFWwindow* window) {
        m_lastKeyPressed = glfwGetKey(window, GLFW_KEY_ESCAPE);
        if (m_lastKeyPressed == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
    }

    void MainWindow::clear() {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void MainWindow::processInput() { __processInput(m_window); }

    void MainWindow::swapBuffers() {
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
    int MainWindow::getLastKeyPressed() const { return m_lastKeyPressed; }

    bool MainWindow::shouldClose() const { return glfwWindowShouldClose(m_window); }

    GLFWwindow* MainWindow::getWindow()
    {
        return m_window;
    }
} // namespace IRC
