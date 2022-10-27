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
    m_lineColor = new glm::vec3(0.12156863f, 0.8862745f, 0.6039216f);
    m_pointColor = new glm::vec3(0.35686275f, 0.078431375f, 0.83137256f);
    m_selectPointColor = new glm::vec3(0.05882353f, 0.73333335f, 0.8352941f);
    m_selectTriangleColor = new glm::vec3(0.82156863f, 0.0862745f, 0.2039216f);
    m_highlightPointColor = new glm::vec3(0.02156863f, 0.9862745f, 0.9039216f);
    m_highlightTriangleColor = new glm::vec3(0.6901961f, 0.14117648f, 0.8980392f);

    // 初始化灯
    m_lampPos = new glm::vec3(3.f, 8.f, 5.f);
    m_lampModelMatrix = glm::mat4(1.f);
    m_lampModelMatrix = glm::translate(m_lampModelMatrix, *m_lampPos);
    m_lampModelMatrix = glm::scale(m_lampModelMatrix, glm::vec3(.3f, .3f, .3f));

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

    delete m_lampPos;
}

void MainRender::render(float deltaTime)
{
    m_deltaTime = deltaTime;
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_camera->updateCameraVectors();

    m_projectionMatrix = glm::perspective(
            glm::radians(m_camera->zoom),
            (float)m_width / (float)m_height,
            NEAR_PLANE, FAR_PLANE);
    m_viewMatrix = m_camera->GetViewMatrix();
    updateModelMatrix();

    if (modelLoaded) {
        renderHighlight();
        if (mode.select) renderSelect();
        if (mode.fill) renderFill();
        if (mode.line) renderLine();
        if (mode.point) renderPoint();
    }

    if (mode.lamp) renderLamp();
}

void MainRender::renderHighlight() {
    m_modelColorShader.use(m_modelMatrix, m_viewMatrix, m_projectionMatrix);
    m_modelColorShader.setValue("modelColor", *m_highlightPointColor);
    m_highlightPoint->render(-1.15f, 5.0f);

    m_modelColorShader.use(m_modelMatrix, m_viewMatrix, m_projectionMatrix);
    m_modelColorShader.setValue("modelColor", *m_highlightTriangleColor);
    m_highlightTriangle->render(-1.25f);
}

void MainRender::renderSelect() {
    if (rayPicker->selectPointValid)
    {
        m_selectPoint->resetIndices(rayPicker->selectMeshIndex, rayPicker->selectPointIndex);
        m_modelColorShader.use(m_modelMatrix, m_viewMatrix, m_projectionMatrix);
        m_modelColorShader.setValue("modelColor", *m_selectPointColor);
        m_selectPoint->render(-1.1f, 5.0f);

    }
    else if (rayPicker->selectFaceValid)
    {
        m_selectTriangle->resetIndices(
                rayPicker->selectMeshIndex,
                rayPicker->selectFaceIndex[0],
                rayPicker->selectFaceIndex[1],
                rayPicker->selectFaceIndex[2]);
        m_modelColorShader.use(m_modelMatrix, m_viewMatrix, m_projectionMatrix);
        m_modelColorShader.setValue("modelColor", *m_selectTriangleColor);
        m_selectTriangle->render(-1.1f);
    }
}

void MainRender::renderFill() {
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);//设置绘制模型为绘制前面与背面模型，以填充的方式绘制
    // don't forget to enable shader before setting uniforms
    m_modelShader.use(m_modelMatrix, m_viewMatrix, m_projectionMatrix);
    m_modelShader.setValue("light.position", glm::vec4(m_lampPos->x, m_lampPos->y, m_lampPos->z, 1.f));
    m_modelShader.setValue("viewPos", m_camera->position);

    m_modelShader.setValue("light.ambient", 0.3f, 0.3f, 0.3f);
    m_modelShader.setValue("light.diffuse", 0.5f, 0.5f, 0.5f);
    m_modelShader.setValue("light.specular", 1.0f, 1.0f, 1.0f);

    m_modelShader.setValue("light.constant", 1.0f);
    m_modelShader.setValue("light.linear", 0.027f);
    m_modelShader.setValue("light.quadratic", 0.0028f);

    m_modelShader.setValue("material.shininess", 64.0f);

    m_model->render(&m_modelShader);
}

void MainRender::renderLine() {
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);//将绘制模式改为线
    glEnable(GL_POLYGON_OFFSET_LINE);//开启多边形偏移
    glLineWidth(1.0f);
    glPolygonOffset(-1.0f,-1.0f);//设置多边形偏移量
    m_modelColorShader.use(m_modelMatrix, m_viewMatrix, m_projectionMatrix);
    m_modelColorShader.setValue("modelColor", *m_lineColor);
    m_model->render(&m_modelColorShader);
    glDisable(GL_POLYGON_OFFSET_LINE);//关闭多边形偏移
}

void MainRender::renderPoint() {
    glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);//将绘制模式改为点
    glEnable(GL_POLYGON_OFFSET_POINT);//开启多边形偏移
    glPolygonOffset(-1.5f,-1.5f);//设置多边形偏移量
    glPointSize(2.5f);
    m_modelColorShader.use(m_modelMatrix, m_viewMatrix, m_projectionMatrix);
    m_modelColorShader.setValue("modelColor", *m_pointColor);
    m_model->render(&m_modelColorShader);
    glDisable(GL_POLYGON_OFFSET_POINT);//关闭多边形偏移
}

void MainRender::renderLamp() {
    m_lampShader.use();
    m_lampShader.setValue("projection", m_projectionMatrix);
    m_lampShader.setValue("view", m_viewMatrix);
    m_lampShader.setValue("model", m_lampModelMatrix);
    m_lampModel->render(&m_lampShader);
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
    initializeRayPicker();

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
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

    // 重置模型矩阵
    resetModelMatrix();
    m_camera->reset();

    modelLoaded = true;
}

void MainRender::resetModelMatrix() {
    modelTransform.position = glm::vec3(0.0f, 0.0f, 0.0f);
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
