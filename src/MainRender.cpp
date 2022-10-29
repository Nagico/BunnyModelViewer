#include "MainRender.h"
#include "util/opengl/Camera.h"
#include "util/opengl/Image.h"
#include "util/opengl/Model.h"
#include "util/opengl/PolygonPoint.h"
#include "util/opengl/PolygonTriangle.h"
#include "util/RayPicker.h"
#include "util/event/Event.h"
#include "util/event/Mouse.h"
#include "util/event/Keyboard.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>


MainRender::MainRender(GLFWwindow *window, Camera *camera, Mouse *mouse, Keyboard *keyboard)
{
    modelLoaded = false;
    m_deltaTime = .0f;

    m_window = window;
    m_camera = camera;
    m_mouse = mouse;
    m_keyboard = keyboard;

    // 初始化色彩
    m_lineColor = new glm::vec3(0.359f, 0.749f, 0.605f);
    m_pointColor = new glm::vec3(0.595f, 0.582f, 0.335f);
    m_selectPointColor = new glm::vec3(0.05882353f, 0.73333335f, 0.8352941f);
    m_selectTriangleColor = new glm::vec3(0.555f, 0.255f, 0.695f);
    m_highlightPointColor = new glm::vec3(0.02156863f, 0.9862745f, 0.9039216f);
    m_highlightTriangleColor = new glm::vec3(0.804f, 0.097f, 0.961f);

    // 初始化矩阵
    m_projectionMatrix = glm::perspective(
            glm::radians(m_camera->zoom),
            (float)m_width / (float)m_height,
            NEAR_PLANE, FAR_PLANE);
    m_viewMatrix = m_camera->GetViewMatrix();

    initializeGL();
    initializeEvent();
}

MainRender::~MainRender()
{
    unloadModel();

    delete m_lampModel;
    delete rayPicker;

    delete m_lineColor;
    delete m_pointColor;
    delete m_selectPointColor;
    delete m_selectTriangleColor;
    delete m_highlightPointColor;
    delete m_highlightTriangleColor;
}

void MainRender::render(float deltaTime)
{
    m_deltaTime = deltaTime;
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_camera->updateCameraVectors();
    lightFactory->refreshTorchLightPos(m_camera->position, m_camera->front);

    // 透视投影
    m_projectionMatrix = glm::perspective(
            glm::radians(m_camera->zoom),
            (float)m_width / (float)m_height,
            NEAR_PLANE, FAR_PLANE);
    // 正交投影
//     m_projectionMatrix = glm::ortho(
//             -5.f / 2.0f, 5.f / 2.0f,
//             -5.f * m_height / m_width / 2.0f, 5.f * m_height / m_width / 2.0f,
//             NEAR_PLANE, FAR_PLANE);
    m_viewMatrix = m_camera->GetViewMatrix();
    updateModelMatrix();

    if (modelLoaded) {
        renderShadow();

        renderHighlight(m_modelColorShader);
        if (mode.select) renderSelect(m_modelColorShader);
        if (mode.fill) renderFill(m_modelShader);
        if (mode.line) renderLine(m_modelColorShader);
        if (mode.point) renderPoint(m_modelColorShader);
    }

    if (mode.lamp) renderLamp(m_lampShader);
}

void MainRender::renderShadow() {
    // 渲染深度贴图
    glm::mat4 lightProjection, lightView;
    lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, NEAR_PLANE, FAR_PLANE);
    lightView = glm::lookAt(lightFactory->getLight(0)->position, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    m_lightSpaceMatrix = lightProjection * lightView;
    // render scene from light's point of view
    m_shadowShader.use();
    m_shadowShader.setValue("lightSpaceMatrix", m_lightSpaceMatrix);
    glCullFace(GL_FRONT);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFbo);
    glClear(GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    renderFill(m_shadowShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCullFace(GL_BACK);
    // reset viewport
    glViewport(0, 0, m_width, m_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void MainRender::renderHighlight(ShaderProgram &shader) {
    shader.use(m_modelMatrix, m_viewMatrix, m_projectionMatrix);
    shader.setValue("modelColor", *m_highlightPointColor);
    m_highlightPoint->render(-1.15f, 5.0f);

    shader.use(m_modelMatrix, m_viewMatrix, m_projectionMatrix);
    shader.setValue("modelColor", *m_highlightTriangleColor);
    m_highlightTriangle->render(-1.25f);
}

void MainRender::renderSelect(ShaderProgram &shader) {
    if (rayPicker->selectPointValid)
    {
        m_selectPoint->resetIndices(rayPicker->selectMeshIndex, rayPicker->selectPointIndex);
        shader.use(m_modelMatrix, m_viewMatrix, m_projectionMatrix);
        shader.setValue("modelColor", *m_selectPointColor);
        m_selectPoint->render(-1.1f, 5.0f);

    }
    else if (rayPicker->selectFaceValid)
    {
        m_selectTriangle->resetIndices(
                rayPicker->selectMeshIndex,
                rayPicker->selectFaceIndex[0],
                rayPicker->selectFaceIndex[1],
                rayPicker->selectFaceIndex[2]);
        shader.use(m_modelMatrix, m_viewMatrix, m_projectionMatrix);
        shader.setValue("modelColor", *m_selectTriangleColor);
        m_selectTriangle->render(-1.1f);
    }
}

void MainRender::renderFill(ShaderProgram &shader) {
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);//设置绘制模型为绘制前面与背面模型，以填充的方式绘制
    // don't forget to enable shader before setting uniforms
    shader.use(m_modelMatrix, m_viewMatrix, m_projectionMatrix);
    shader.setValue("viewPos", m_camera->position);
    shader.setValue("material.shininess", defaultShininess);
    shader.setValue("lightSpaceMatrix", m_lightSpaceMatrix);
    lightFactory->importShaderValue(shader);


    m_model->render(&shader, false, false, m_depthMap);
}

void MainRender::renderLine(ShaderProgram &shader) {
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);//将绘制模式改为线
    glEnable(GL_POLYGON_OFFSET_LINE);//开启多边形偏移
    glLineWidth(1.0f);
    glPolygonOffset(-1.0f,-1.0f);//设置多边形偏移量
    shader.use(m_modelMatrix, m_viewMatrix, m_projectionMatrix);
    shader.setValue("modelColor", *m_lineColor);
    m_model->render(&shader);
    glDisable(GL_POLYGON_OFFSET_LINE);//关闭多边形偏移
}

void MainRender::renderPoint(ShaderProgram &shader) {
    glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);//将绘制模式改为点
    glEnable(GL_POLYGON_OFFSET_POINT);//开启多边形偏移
    glPolygonOffset(-1.5f,-1.5f);//设置多边形偏移量
    glPointSize(2.5f);
    shader.use(m_modelMatrix, m_viewMatrix, m_projectionMatrix);
    shader.setValue("modelColor", *m_pointColor);
    m_model->render(&shader);
    glDisable(GL_POLYGON_OFFSET_POINT);//关闭多边形偏移
}

void MainRender::renderLamp(ShaderProgram &shader) {
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
//    m_lampShader.use(lightFactory->getLight(0)->modelMatrix, m_viewMatrix, m_projectionMatrix);
//    m_lampShader.setValue("lightColor", lightFactory->getLight(0)->color);
//    m_lampModel->prepareRender(&m_lampShader);
    lightFactory->modelRender(shader, m_viewMatrix, m_projectionMatrix);
}

void MainRender::resizeGL(int w, int h)
{
    m_width = w;
    m_height = h;
    GLfloat aspect = (GLfloat)w / (GLfloat)h;
    m_projectionMatrix = glm::perspective(glm::radians(m_camera->zoom), aspect, NEAR_PLANE, FAR_PLANE);
}

void MainRender::initializeGL() {
    initializeShader();
    initializeModel();
    initializeLight();
    initializeRayPicker();
    initializeShadow();

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
}

void MainRender::initializeShadow() {
    // 深度贴图帧缓冲对象
    glGenFramebuffers(1, &m_depthMapFbo);
    // 创建2D纹理
    glGenTextures(1, &m_depthMap);
    glBindTexture(GL_TEXTURE_2D, m_depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // 深度纹理作为帧缓冲的深度缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MainRender::initializeShader() {
#pragma omp parallel
#pragma omp sections
    {
#pragma omp section
        {
            m_modelShader.load("assets/shader/model_vertex.glsl", "assets/shader/model_fragment.glsl");
        }
#pragma omp section
        {
            m_modelColorShader.load("assets/shader/model_vertex.glsl", "assets/shader/model_color_fragment.glsl");
        }
#pragma omp section
        {
            m_lampShader.load("assets/shader/lamp_vertex.glsl", "assets/shader/lamp_fragment.glsl");
        }
#pragma omp section
        {
            m_shadowShader.load("assets/shader/depth_shadow_vertex.glsl", "assets/shader/depth_shadow_fragment.glsl");
        }
    }
}

void MainRender::initializeModel() {
    m_lampModel = new Model("assets/model/sphere.ply");
}

void MainRender::initializeRayPicker() {
    rayPicker = new RayPicker();
}

void MainRender::initializeEvent() {
    auto &handler = EventHandler::get();

    initializeRayPickerEvent(handler);
    initializeCameraEvent(handler);
    initializeModelEvent(handler);
    initializeModeChangeEvent(handler);
}

void MainRender::initializeRayPickerEvent(EventHandler& handler) {

    // Ctrl+左键 输出当前选中几何元素信息
    handler.addListener([this](const event::Mouse::ClickHoldEvent<MouseButton::LEFT>& event) {
        if (mode.select && !mode.gui) {
            if (rayPicker->selectPointValid) {
                // TODO: 事件驱动，自定义内容
                std::cout << "select point (" << rayPicker->selectPointIndex << "): "
                    << rayPicker->selectPoint.position.x << ", " << rayPicker->selectPoint.position.y << ", "
                    << rayPicker->selectPoint.position.z << std::endl;
            }
            else if (rayPicker->selectFaceValid) {
                // TODO: 事件驱动，自定义内容
                std::cout << "select face: " << std::endl;
                for (int i = 0; i < 3; i++) {
                    std::cout << "\t point (" << rayPicker->selectFaceIndex[i] << "): "
                              << rayPicker->selectFace[i].position.x << ", " << rayPicker->selectFace[i].position.y
                              << ", " << rayPicker->selectFace[i].position.z << std::endl;
                }

            } else {
                std::cout << "select nothing" << std::endl;
            }

        }
    });

    // Ctrl+右键 高亮/取消高亮当前元素
    handler.addListener([this](const event::Mouse::ClickHoldEvent<MouseButton::RIGHT>& event) {
        if (mode.select && !mode.gui) {
            if (rayPicker->selectPointValid) {
                // TODO: 事件驱动，自定义内容
                auto status = m_highlightPoint->modifyIndices(rayPicker->selectMeshIndex, rayPicker->selectPointIndex);
                std::cout << (status ? "" : "un") << "highlight point (" << rayPicker->selectPointIndex << "): "
                    << rayPicker->selectPoint.position.x << ", " << rayPicker->selectPoint.position.y << ", "
                    << rayPicker->selectPoint.position.z << std::endl;
            }
            else if (rayPicker->selectFaceValid) {
                // TODO: 事件驱动，自定义内容
                auto status = m_highlightTriangle->modifyIndices(
                        rayPicker->selectMeshIndex,
                        rayPicker->selectFaceIndex[0],
                        rayPicker->selectFaceIndex[1],
                        rayPicker->selectFaceIndex[2]);
                std::cout << (status ? "" : "un") << "highlight face: " << std::endl;
                for (int i = 0; i < 3; i++) {
                    std::cout << "\t point (" << rayPicker->selectFaceIndex[i] << "): "
                              << rayPicker->selectFace[i].position.x << ", " << rayPicker->selectFace[i].position.y
                              << ", " << rayPicker->selectFace[i].position.z << std::endl;
                }

            } else {
                std::cout << "highlight nothing" << std::endl;
            }

        }
    });

    // Ctrl 预览当前选中元素
    handler.addListener([this](const event::Mouse::MoveEvent &event){
        if (modelLoaded && mode.select && !mode.gui) {
            rayPicker->rayPick(
                    m_model->meshes, m_camera->position,
                    m_modelMatrix, m_viewMatrix, m_projectionMatrix,
                    (float)event.position.x, (float)event.position.y, m_width, m_height);

        }
    });


}

void MainRender::initializeCameraEvent(EventHandler& handler) {
    // R 重置相机
    handler.addListener([this](const event::Keyboard::KeyPressEvent<KeyboardKey::R> &event){
        m_camera->reset();
    });

    // W 相机前进
    handler.addListener([this](const event::Keyboard::KeyPressEvent<KeyboardKey::W> &event){
        m_camera->ProcessKeyboard(Camera::FORWARD, m_deltaTime);
    });

    // S 相机后退
    handler.addListener([this](const event::Keyboard::KeyPressEvent<KeyboardKey::S> &event){
        m_camera->ProcessKeyboard(Camera::BACKWARD, m_deltaTime);
    });

    // A 相机左移
    handler.addListener([this](const event::Keyboard::KeyPressEvent<KeyboardKey::A> &event){
        m_camera->ProcessKeyboard(Camera::LEFT, m_deltaTime);
    });

    // D 相机右移
    handler.addListener([this](const event::Keyboard::KeyPressEvent<KeyboardKey::D> &event){
        m_camera->ProcessKeyboard(Camera::RIGHT, m_deltaTime);
    });

    // Q 相机上移
    handler.addListener([this](const event::Keyboard::KeyPressEvent<KeyboardKey::Q> &event){
        m_camera->ProcessKeyboard(Camera::UP, m_deltaTime);
    });

    // E 相机下移
    handler.addListener([this](const event::Keyboard::KeyPressEvent<KeyboardKey::E> &event){
        m_camera->ProcessKeyboard(Camera::DOWN, m_deltaTime);
    });

    // 相机模式 左键 拖拽相机
    handler.addListener([this](const event::Mouse::MoveEvent& event) {
        if (mode.camera) {
            m_camera->ProcessMouseMovement(event.offset.x, event.offset.y);
        }
    });

    // 相机模式 滚轮 调整FOV
    handler.addListener([this](const event::Mouse::ScrollEvent& event) {
        if (mode.camera) {
            m_camera->ProcessMouseScroll(event.offset.y);
        }
    });
}

void MainRender::initializeModelEvent(EventHandler& handler) {
    // R 重置模型
    handler.addListener([this](const event::Keyboard::KeyPressEvent<KeyboardKey::R> &event){
        resetModelMatrix();
    });

    // 鼠标滚轮 缩放模型
    handler.addListener([this](const event::Mouse::ScrollEvent &event) {
        if (modelLoaded && !mode.camera && !mode.gui) {
            auto scale = modelTransform.scale + event.offset.y * 0.03f;
            if (scale < 0.1f) {
                scale = 0.1f;
            }
            modelTransform.scale = scale;
        }
    });

    // 鼠标左键 旋转模型
    handler.addListener([this](const event::Mouse::MoveEvent &event) {
        if (!mode.camera && m_mouse->state[MouseButton::LEFT] && !mode.gui) {
            if (modelLoaded) {
                modelTransform.rotation.x -= event.offset.y * 0.005f / modelTransform.scale;
                modelTransform.rotation.y += event.offset.x * 0.005f / modelTransform.scale;
            }
        }
    });

    // 鼠标右键 平移模型
    handler.addListener([this](const event::Mouse::MoveEvent &event) {
        if (!mode.camera && m_mouse->state[MouseButton::RIGHT] && !mode.gui) {
            if (modelLoaded) {
                modelTransform.position.x += event.offset.x * 0.0004f / modelTransform.scale;
                modelTransform.position.y += event.offset.y * 0.0004f / modelTransform.scale;
            }
        }
    });
}

void MainRender::initializeModeChangeEvent(EventHandler& handler) {
    // F 开关模型填充
    handler.addListener([this](const event::Keyboard::KeyHoldEvent<KeyboardKey::F> &event){
        mode.fill = !mode.fill;
    });

    // L 开关模型线框
    handler.addListener([this](const event::Keyboard::KeyHoldEvent<KeyboardKey::L> &event){
        mode.line = !mode.line;
    });

    // P 开关模型点
    handler.addListener([this](const event::Keyboard::KeyHoldEvent<KeyboardKey::P> &event){
        mode.point = !mode.point;
    });

    // C 开关相机模式
    handler.addListener([this](const event::Keyboard::KeyHoldEvent<KeyboardKey::C> &event){
        mode.camera = !mode.camera;
    });

    // Ctrl 进入选择模式
    handler.addListener([this](const event::Keyboard::KeyHoldEvent<KeyboardKey::LEFT_CONTROL> &event){
        mode.select = true;
    });
    handler.addListener([this](const event::Keyboard::KeyReleaseEvent<KeyboardKey::LEFT_CONTROL> &event){
        mode.select = false;
    });

    // gui模式控制 鼠标点击时
    handler.addListener([this](const event::Mouse::ClickHoldEvent<MouseButton::LEFT> &event){
        mode.gui = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
    });
    handler.addListener([this](const event::Mouse::ClickReleaseEvent<MouseButton::LEFT> &event){
        mode.gui = false;
    });
}

void MainRender::loadModel(const string &path) {
    if (modelLoaded) {  // 释放之前的模型
        delete m_model;
        delete m_selectPoint;
        delete m_selectTriangle;
        delete m_highlightPoint;
        delete m_highlightTriangle;

        modelLoaded = false;
    }

    // 加载模型
    try {
        m_model = new Model(path);
    }
    catch (std::runtime_error &ex) {
        std::cerr << ex.what() << std::endl;
        return;
    }

    if (m_model->meshes.empty()) {
        std::cerr << "Model is empty" << std::endl;
        return;
    }
    modelName = path.substr(path.find_last_of('/') + 1);

    // 加载几何模型

    m_selectPoint = new PolygonPoint(m_model->meshes);
    m_selectTriangle = new PolygonTriangle(m_model->meshes);
    m_highlightPoint = new PolygonPoint(m_model->meshes);
    m_highlightTriangle = new PolygonTriangle(m_model->meshes);

    defaultShininess = m_model->meshes[0].getMeshInfo().valid ? m_model->meshes[0].getMeshInfo().shininess : 32.0f;

    // 重置模型矩阵
    resetModelMatrix();
    m_camera->reset();

    modelLoaded = true;
}

void MainRender::resetModelMatrix() {
    modelTransform.position = glm::vec3(0.04f, 0.0f, 0.0f);
    modelTransform.scale = 1.f;
    modelTransform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    updateModelMatrix();
}

void MainRender::updateModelMatrix() {
    m_modelMatrix = m_model->basisTransform;
    m_modelMatrix = glm::translate(m_modelMatrix, modelTransform.position);
    m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(modelTransform.scale));
    m_modelMatrix = glm::rotate(m_modelMatrix, modelTransform.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    m_modelMatrix = glm::rotate(m_modelMatrix, modelTransform.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    m_modelMatrix = glm::rotate(m_modelMatrix, modelTransform.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
}

void MainRender::unloadModel() {
    if (modelLoaded) {
        delete m_model;
        delete m_selectPoint;
        delete m_selectTriangle;
        delete m_highlightPoint;
        delete m_highlightTriangle;

        modelLoaded = false;
    }
}

std::string MainRender::getHighlightPointString() const& {
    return m_highlightPoint->getIndicesString();
}

std::string MainRender::getHighlightTriangleString() const &{
    return m_highlightTriangle->getIndicesString();
}

void MainRender::initializeLight() {
    lightFactory = &LightFactory::get();
    lightFactory->setBaseModel(m_lampModel);
    lightFactory->reset();

}
