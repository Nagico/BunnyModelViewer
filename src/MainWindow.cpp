#include "MainWindow.h"
#include "MainRender.h"
#include "util/opengl/Camera.h"
#include "util/event/Mouse.h"
#include "util/event/Keyboard.h"
#include "util/RayPicker.h"
#include "util/nfd/nfd.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <iostream>
#include <sstream>

MainWindow::MainWindow(int width, int height) : OpenGLWindow(width, height)
{
    m_camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    m_mouse = new Mouse();
    m_keyboard = new Keyboard();

    m_render = new MainRender(m_window, m_camera, m_mouse, m_keyboard);

    initializeImGui();

    initializeEvent();

    resizeEvent(m_width, m_height);
}

MainWindow::~MainWindow() {
    if (m_camera)
        delete m_camera;
    if (m_render)
        delete m_render;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void MainWindow::render(float deltaTime) {
    EventHandler::get().getEventBus()->process();
    m_mouse->update();
    m_keyboard->update();

    if (m_render->mode.camera)
        setCursorMode(CursorMode::Disabled);
    else
        setCursorMode(CursorMode::Normal);

    renderImGui();

    m_render->render(deltaTime);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    refreshTitle();

}

void MainWindow::refreshTitle() {
    std::stringstream title;
    if (m_render->modelLoaded) {
        if (m_render->mode.select) title << "[Select] ";
        if (m_render->mode.camera) title << "[Camera] ";
        title << m_render->modelName << " | ";
        if (m_render->mode.fill) title << "Fill ";
        if (m_render->mode.line) title << "Line ";
        if (m_render->mode.point) title << "Point ";
    } else {
        title << "No Model Loaded";
    }
    m_title = title.str();
}

void MainWindow::initializeEvent() {
    auto &handler = EventHandler::get();

    handler.addListener([this](const event::Keyboard::KeyHoldEvent<KeyboardKey::ECS> &event) {
        if (m_render->mode.select) { m_render->mode.select = false; return; }
        if (m_render->mode.camera) { m_render->mode.camera = false; return; }

        close();
    });
}

void MainWindow::resizeEvent(int width, int height) {
    OpenGLWindow::resizeEvent(width, height);
    m_render->resizeGL(width, height);
}

void MainWindow::keyEvent(int key, int scancode, int action, int mods) {
    m_keyboard->keyCallback(key, scancode, action, mods);
}

void MainWindow::mouseMoveEvent(double xpos, double ypos) {
    m_mouse->cursorPosCallback(xpos, ypos);
}

void MainWindow::mouseScrollEvent(double xoffset, double yoffset) {
    m_mouse->scrollCallback(xoffset, yoffset);
}

void MainWindow::mouseButtonEvent(int button, int action, int mods) {
    m_mouse->mouseButtonCallback(button, action, mods);
}

bool MainWindow::loadModel(const string &path) {
    m_render->loadModel(path);
    return m_render->modelLoaded;
}

bool MainWindow::unloadModel() {
    m_render->unloadModel();
    return !m_render->modelLoaded;
}

void MainWindow::initializeImGui() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 430 core");
}

void MainWindow::renderImGui() {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    m_render->mode.gui = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
    //ImGui::ShowDemoWindow();

    {
        ImGui::Begin("Control", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None);

        if (ImGui::BeginTabItem("Model")) {
            if (m_render->modelLoaded) {
                ImGui::Checkbox("Fill (F)", &m_render->mode.fill);
                ImGui::SameLine();
                ImGui::Checkbox("Line (L)", &m_render->mode.line);
                ImGui::SameLine();
                ImGui::Checkbox("Point (P)", &m_render->mode.point);
                ImGui::SliderFloat3("Position", glm::value_ptr(m_render->modelTransform.position), -.5f, .5f);
                ImGui::SliderFloat3("Rotate", glm::value_ptr(m_render->modelTransform.rotation), -3.1415f, 3.1415f);
                ImGui::SliderFloat("Scale", &m_render->modelTransform.scale, 0.1f, 5.0f);
                if (ImGui::Button("Reset")) {
                    m_render->resetModelMatrix();
                }
            }

            if (ImGui::Button("Open Model")) {
                auto path = openFile();
                if (path != "") {
                    loadModel(path);
                }
            }

            if (m_render->modelLoaded) {
                ImGui::SameLine();
                if (ImGui::Button("Unload Model")) {
                    unloadModel();
                }
            }

            if (ImGui::TreeNode("Built-in models with texture"))
            {
                if (ImGui::Button("Bunny 1")) {
                    loadModel("assets/model/bunny_texture/bunny.obj");
                }
                ImGui::SameLine();
                if (ImGui::Button("Bunny 2")) {
                    loadModel("assets/model/bunny/bunny.obj");
                }
                if (ImGui::Button("Nanosuit")) {
                    loadModel("assets/model/nanosuit/nanosuit.obj");
                }
                ImGui::TreePop();
                ImGui::Separator();
            }

            if (ImGui::TreeNode("Built-in models without texture"))
            {
                if (ImGui::Button("Bunny zipper")) {
                    loadModel("assets/model/bun_zipper.ply");
                }
                ImGui::SameLine();
                if (ImGui::Button("Bunny iH")) {
                    loadModel("assets/model/bunny_iH.ply2");
                }
                if (ImGui::Button("Cube 1")) {
                    loadModel("assets/model/cube.ply");
                }
                ImGui::SameLine();
                if (ImGui::Button("Cube 2")) {
                    loadModel("assets/model/cube.obj");
                }
                ImGui::SameLine();
                if (ImGui::Button("Sphere")) {
                    loadModel("assets/model/sphere.ply");
                }
                ImGui::TreePop();
                ImGui::Separator();
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Camera")) {
            ImGui::Checkbox("Camera Mode (C)", &m_render->mode.camera);
            ImGui::SliderFloat3("Position", glm::value_ptr(m_camera->position), -5.f, 5.f);
            ImGui::SliderFloat("Yaw", &m_camera->yaw, -180.f, 180.f);
            ImGui::SliderFloat("Pitch", &m_camera->pitch, -180.f, 180.f);
            ImGui::SliderFloat("Zoom", &m_camera->zoom, 1.f, 90.f);

            if (ImGui::Button("Reset")) {
                m_camera->reset();
            }
            ImGui::EndTabItem();
        }

        if (m_render->modelLoaded) {
            if (ImGui::BeginTabItem("Select")) {
                ImGui::Checkbox("Select Mode (Ctrl)", &m_render->mode.select);
                if(m_render->mode.select) {
                    ImGui::Text("Selected");
                    auto picker = m_render->rayPicker;
                    if (picker->selectPointValid) {
                        ImGui::Text("Point (%d): %.4f, %.4f, %.4f", picker->selectPointIndex,
                                    picker->selectPoint.position.x,
                                    picker->selectPoint.position.y,
                                    picker->selectPoint.position.z);
                    }
                    else if (picker->selectFaceValid) {
                        ImGui::Text("Face: ");
                        for (int i = 0; i < 3; i++)
                            ImGui::Text("Point(%d) (%d): %.4f, %.4f, %.4f", i,
                                        picker->selectFaceIndex[i],
                                        picker->selectFace[i].position.x,
                                        picker->selectFace[i].position.y,
                                        picker->selectFace[i].position.z);
                    }
                    else {
                        ImGui::Text("Nothing");
                    }
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("HighLight")) {
                ImGui::Text("Points");
                ImGui::Text("%s", m_render->getHighlightPointString().c_str());
                ImGui::Text("Triangles");
                ImGui::Text("%s", m_render->getHighlightTriangleString().c_str());
                ImGui::EndTabItem();
            }
        }

        ImGui::EndTabBar();

        ImGui::End();
    }

    // Rendering
    ImGui::Render();
}

std::string MainWindow::openFile() const& {
    nfdchar_t *outPath = nullptr;
    nfdresult_t result = NFD_OpenDialog(nullptr, nullptr, &outPath);
    if (result == NFD_OKAY) {
        std::string path(outPath);
        free(outPath);
        return path;
    }
    return "";
}