#include "GUIBase.h"

#include "Graphics/MainWindow.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

IRC::GUIBase::~GUIBase()
{
}

void setup(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char* glsl_version = "#version 130";

    ImGui_ImplOpenGL3_Init(glsl_version);
}

IRC::GUIBase::GUIBase(MainWindow* parent) : Node(parent)
{
    static bool settingUp = false;
    if (!settingUp) {
        setup(parent->getWindow());
        settingUp = true;
    }
}

void IRC::GUIBase::update()
{

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


}

void IRC::GUIBase::startFrame()
{
}

void IRC::GUIBase::renderFrame()
{
}
