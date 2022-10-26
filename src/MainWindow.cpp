#include "MainWindow.h"
#include "MainRender.h"
#include "util/opengl/Camera.h"
#include "util/event/Mouse.h"
#include "util/event/Keyboard.h"
#include <iostream>
#include <sstream>

MainWindow::MainWindow(int width, int height) : OpenGLWindow(width, height)
{
    m_camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    m_mouse = new Mouse();
    m_keyboard = new Keyboard();

    m_render = new MainRender(m_window, m_camera, m_mouse, m_keyboard);

    initializeEvent();

    resizeEvent(m_width, m_height);
}

MainWindow::~MainWindow() {
    if (m_camera)
        delete m_camera;
    if (m_render)
        delete m_render;
}

void MainWindow::render(float deltaTime) {
    EventHandler::get().getEventBus()->process();
    m_mouse->update();
    m_keyboard->update();
    m_render->render(deltaTime);

    refreshTitle();

}

void MainWindow::refreshTitle() {
    std::stringstream title;
    if (m_render->modelLoaded) {
        if (m_render->mode.select) title << "[Select] ";
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

    handler.addListener([this](const event::Keyboard::KeyPressEvent<KeyboardKey::ECS> &event) {
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
