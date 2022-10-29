//
// Created by co on 2022/10/29.
//

#include "Controller.h"

#include "opengl/Camera.h"
#include "event/Mouse.h"
#include "event/Keyboard.h"
#include "RayPicker.h"
#include "nfd/nfd.h"
#include "../MainRender.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include <Windows.h>

void Controller::initialize(GLFWwindow *window) {
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
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430 core");

    auto hd = GetDesktopWindow();
    auto zoom = GetDpiForWindow(hd);
    ImGui::GetStyle().ScaleAllSizes(zoom / 96.0f);
    auto fontSize = zoom / 96.0f * 15.0f;
    // font size
    io.Fonts->AddFontFromFileTTF("assets/font/calibri.ttf", fontSize);
}

void Controller::prepareRender() {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //gui模式控制 hover
    if(ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
        m_render->mode.gui = true;
    if (m_render->mode.gui && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && !m_mouse->state[MouseButton::LEFT])
        m_render->mode.gui = false;

    // ImGui::ShowDemoWindow();

    {
        ImGui::Begin("Control", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None);

        if (ImGui::BeginTabItem("Model")) {
            showModelTab();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Camera")) {
            showCameraTab();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Light")) {
            showLightTab();
            ImGui::EndTabItem();
        }

        if (m_render->modelLoaded) {
            if (ImGui::BeginTabItem("Select")) {
                showSelectTab();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("HighLight")) {
                showHighlightTab();
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
        ImGui::End();
    }

    // Rendering
    ImGui::Render();
}

void Controller::showHighlightTab() const {
    ImGui::Text("Points");
    ImGui::ColorEdit3("Point Color", glm::value_ptr(*m_render->m_highlightPointColor));
    ImGui::Text("%s", m_render->getHighlightPointString().c_str());
    ImGui::Separator();
    ImGui::Text("Triangles");
    ImGui::ColorEdit3("Face Color", glm::value_ptr(*m_render->m_highlightTriangleColor));
    ImGui::Text("%s", m_render->getHighlightTriangleString().c_str());
}

void Controller::showSelectTab() const {
    ImGui::Checkbox("Select Mode (Ctrl)", &m_render->mode.select);
    ImGui::ColorEdit3("Point Color", glm::value_ptr(*m_render->m_selectPointColor));
    ImGui::ColorEdit3("Face Color", glm::value_ptr(*m_render->m_selectTriangleColor));
    ImGui::Separator();
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
}

void Controller::showLightTab() {
    static int imguiLightType[MAX_LIGHT_NUM];

    for (int i=0; i<MAX_LIGHT_NUM; i++) {
        auto &light = *(m_render->lightFactory->getLight(i));
        string status =
                light.type == NONE ? "Disable" :
                light.type == DIRECTIONAL_LIGHT ? "Directional" :
                light.type == POINT_LIGHT ? "Point" :
                light.type == SPOT_LIGHT ? "Spot" :
                light.type == TORCH_LIGHT ? "Torch" : "";

        auto lightTreeNode = [i, status] () {
            bool open = ImGui::TreeNodeEx(("Light " + std::to_string(i) + " ").c_str(), i == 0 ? ImGuiTreeNodeFlags_DefaultOpen : 0);
            ImGui::SameLine();
            ImGui::Text("%s", status.c_str());
            return open;
        };
        if (lightTreeNode()) {
            imguiLightType[i] = light.type;
            ImGui::Combo("Type", &imguiLightType[i], "Disable\0Directional\0Point\0Spot\0Torch\0");
            if (imguiLightType[i] != light.type) {
                light.type = static_cast<LightType>(imguiLightType[i]);
                light.reset();
            }

            switch (light.type) {
                case DIRECTIONAL_LIGHT:
                    ImGui::ColorEdit3("Color", glm::value_ptr(light.color));
                    ImGui::DragFloat3("Direction", glm::value_ptr(light.direction), 0.01f, -10.f, 10.f);
                    ImGui::Checkbox("Show Model", &light.isShowModel);
                    ImGui::Text("Phong Model");
                    ImGui::DragFloat("Ambient", &light.ambientX, 0.001f, 0.f, 1.f);
                    ImGui::DragFloat("Diffuse", &light.diffuseX, 0.001f, 0.f, 1.f);
                    ImGui::DragFloat("Specular", &light.specularX, 0.001f, 0.f, 1.f);
                    if (ImGui::Button("Reset"))
                        light.reset();
                    break;
                case POINT_LIGHT:
                    ImGui::ColorEdit3("Color", glm::value_ptr(light.color));
                    ImGui::DragFloat3("Position", glm::value_ptr(light.position), 0.01f, -10.f, 10.f);
                    ImGui::Checkbox("Show Model", &light.isShowModel);
                    ImGui::Text("Phong Model");
                    ImGui::DragFloat("Ambient", &light.ambientX, 0.001f, 0.f, 1.f);
                    ImGui::DragFloat("Diffuse", &light.diffuseX, 0.001f, 0.f, 1.f);
                    ImGui::DragFloat("Specular", &light.specularX, 0.001f, 0.f, 1.f);
                    ImGui::Text("Attenuation");
                    ImGui::DragFloat("Constant", &light.constant, 0.001f, 0.f, 1.f);
                    ImGui::DragFloat("Linear", &light.linear, 0.001f, 0.f, .2f);
                    ImGui::DragFloat("Quadratic", &light.quadratic, 0.001f, 0.f, .1f);
                    if (ImGui::Button("Reset"))
                        light.reset();
                    break;
                case SPOT_LIGHT:
                    ImGui::ColorEdit3("Color", glm::value_ptr(light.color));
                    ImGui::DragFloat3("Position", glm::value_ptr(light.position), 0.01f, -10.f, 10.f);
                    ImGui::Checkbox("Show Model", &light.isShowModel);
                    ImGui::Text("Phong Model");
                    ImGui::DragFloat("Ambient", &light.ambientX, 0.001f, 0.f, 1.f);
                    ImGui::DragFloat("Diffuse", &light.diffuseX, 0.001f, 0.f, 1.f);
                    ImGui::DragFloat("Specular", &light.specularX, 0.001f, 0.f, 1.f);
                    ImGui::Text("Attenuation");
                    ImGui::DragFloat("Constant", &light.constant, 0.001f, 0.f, 1.f);
                    ImGui::DragFloat("Linear", &light.linear, 0.001f, 0.f, .2f);
                    ImGui::DragFloat("Quadratic", &light.quadratic, 0.001f, 0.f, .1f);
                    ImGui::Text("Spotlight");
                    ImGui::DragFloat3("Direction", glm::value_ptr(light.direction), 0.01f, -5.f, 5.f);
                    ImGui::DragFloat("CutOff", &light.cutOffDegree, 0.005f, 0.f, 20.f);
                    ImGui::DragFloat("OuterCutOff", &light.outerCutOffDegree, 0.005f, 0.f, 20.f);
                    if (ImGui::Button("Reset"))
                        light.reset();
                    break;
                case TORCH_LIGHT:
                    ImGui::ColorEdit3("Color", glm::value_ptr(light.color));
                    ImGui::Text("Phong Model");
                    ImGui::DragFloat("Ambient", &light.ambientX, 0.001f, 0.f, 1.f);
                    ImGui::DragFloat("Diffuse", &light.diffuseX, 0.001f, 0.f, 1.f);
                    ImGui::DragFloat("Specular", &light.specularX, 0.001f, 0.f, 1.f);
                    ImGui::Text("Attenuation");
                    ImGui::DragFloat("Constant", &light.constant, 0.001f, 0.f, 1.f);
                    ImGui::DragFloat("Linear", &light.linear, 0.001f, 0.f, .2f);
                    ImGui::DragFloat("Quadratic", &light.quadratic, 0.001f, 0.f, .1f);
                    ImGui::Text("Spotlight");
                    ImGui::DragFloat("CutOff", &light.cutOffDegree, 0.005f, 0.f, 20.f);
                    ImGui::DragFloat("OuterCutOff", &light.outerCutOffDegree, 0.005f, 0.f, 20.f);
                    if (ImGui::Button("Reset"))
                        light.reset();
                default:
                    break;
            }
            ImGui::TreePop();
            ImGui::Separator();
        }
    }

    if (ImGui::Button("Reset All"))
        m_render->lightFactory->reset();
}

Controller::~Controller() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

Controller::Controller(MainRender *render, Mouse *mouse, Camera *camera) {
    m_render = render;
    m_mouse = mouse;
    m_camera = camera;
}

void Controller::render() {
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

std::string Controller::openFile() const& {
    nfdchar_t *outPath = nullptr;
    nfdresult_t result = NFD_OpenDialog(nullptr, nullptr, &outPath);
    if (result == NFD_OKAY) {
        std::string path(outPath);
        free(outPath);
        return path;
    }
    return "";
}

void Controller::showModelTab() const {
    if (m_render->modelLoaded) {
        ImGui::Checkbox("Fill (F)", &m_render->mode.fill);
        ImGui::SameLine();
        ImGui::Checkbox("Line (L)", &m_render->mode.line);
        ImGui::SameLine();
        ImGui::Checkbox("Point (P)", &m_render->mode.point);
        ImGui::Separator();
        ImGui::DragFloat3("Position", glm::value_ptr(m_render->modelTransform.position), 0.005f, -20.f, 20.f);
        ImGui::DragFloat3("Rotate", glm::value_ptr(m_render->modelTransform.rotation), 0.005f, -3.1415f, 3.1415f);
        ImGui::DragFloat("Scale", &m_render->modelTransform.scale, 0.01f, 0.1f, 5.0f);
        ImGui::Separator();
        ImGui::DragFloat("shininess", &m_render->defaultShininess, 0.1f, 0.0f, 100.0f);
        if (ImGui::Button("Reset")) {
            m_render->resetModelMatrix();
        }
    }

    ImGui::Separator();

    ImGui::ColorEdit3("Background", glm::value_ptr(m_render->backgroundColor));
    ImGui::ColorEdit3("Line Color", glm::value_ptr(*m_render->m_lineColor));
    ImGui::ColorEdit3("Point Color", glm::value_ptr(*m_render->m_pointColor));

    ImGui::Separator();

    if (ImGui::Button("Open Model")) {
        auto path = openFile();
        if (path != "") {
            m_render->loadModel(path);
        }
    }

    if (m_render->modelLoaded) {
        ImGui::SameLine();
        if (ImGui::Button("Unload Model")) {
            m_render->unloadModel();
        }
    }

    if (ImGui::TreeNode("Built-in models with texture"))
    {
        if (ImGui::Button("Bunny 1")) {
            m_render->loadModel("assets/model/bunny_texture/bunny.obj");
        }
        ImGui::SameLine();
        if (ImGui::Button("Bunny 2")) {
            m_render->loadModel("assets/model/bunny/bunny.obj");
        }
        if (ImGui::Button("Nanosuit")) {
            m_render->loadModel("assets/model/nanosuit/nanosuit.obj");
        }
        ImGui::TreePop();
        ImGui::Separator();
    }

    if (ImGui::TreeNode("Built-in models without texture"))
    {
        if (ImGui::Button("Bunny zipper")) {
            m_render->loadModel("assets/model/bun_zipper.ply");
        }
        ImGui::SameLine();
        if (ImGui::Button("Bunny iH")) {
            m_render->loadModel("assets/model/bunny_iH.ply2");
        }
        if (ImGui::Button("Cube 1")) {
            m_render->loadModel("assets/model/cube.ply");
        }
        ImGui::SameLine();
        if (ImGui::Button("Cube 2")) {
            m_render->loadModel("assets/model/cube.obj");
        }
        ImGui::SameLine();
        if (ImGui::Button("Sphere")) {
            m_render->loadModel("assets/model/sphere.ply");
        }
        ImGui::TreePop();
        ImGui::Separator();
    }
}

void Controller::showCameraTab() const {
    ImGui::Checkbox("Camera Mode (C)", &m_render->mode.camera);
    ImGui::Separator();
    ImGui::DragFloat3("Position", glm::value_ptr(m_camera->position), 0.005f, -5.f, 5.f);
    ImGui::DragFloat("Yaw", &m_camera->yaw, 0.1f, -180.f, 180.f);
    ImGui::DragFloat("Pitch", &m_camera->pitch, 0.1f, -180.f, 180.f);
    ImGui::DragFloat("Zoom", &m_camera->zoom, 0.05f, 1.f, 90.f);

    if (ImGui::Button("Reset")) {
        m_camera->reset();
    }
}
