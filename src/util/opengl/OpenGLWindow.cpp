#include "OpenGLWindow.h"
#include <iostream>
#include <sstream>
#include <sysinfoapi.h>

int OpenGLWindow::m_major = 3;
int OpenGLWindow::m_minor = 3;
OpenGLWindow::OpenGLProfile OpenGLWindow::m_profile = Core;
int OpenGLWindow::m_samples = 4;

OpenGLWindow::OpenGLWindow(int width, int height)
{
    m_width = width;
    m_height = height;
    initializeWindow();
}

OpenGLWindow::~OpenGLWindow() = default;

void OpenGLWindow::closeCallback(GLFWwindow *window)
{
    auto *_this = static_cast<OpenGLWindow *>(glfwGetWindowUserPointer(window));
    _this->closeEvent();
}

void OpenGLWindow::focusCallback(GLFWwindow *window, int focused)
{
    auto *_this = static_cast<OpenGLWindow *>(glfwGetWindowUserPointer(window));
    _this->focusEvent(focused);
}

void OpenGLWindow::minimizeCallback(GLFWwindow *window, int minimized)
{
    auto *_this = static_cast<OpenGLWindow *>(glfwGetWindowUserPointer(window));
    _this->minimizeEvent(minimized);
}

void OpenGLWindow::posCallback(GLFWwindow* window, int xpos, int ypos)
{
    auto *_this = static_cast<OpenGLWindow *>(glfwGetWindowUserPointer(window));
    _this->posEvent(xpos, ypos);
}

void OpenGLWindow::updateCallback(GLFWwindow* window)
{
    auto *_this = static_cast<OpenGLWindow *>(glfwGetWindowUserPointer(window));
    _this->updateEvent();
}

void OpenGLWindow::resizeCallback(GLFWwindow *window, int width, int height)
{
    auto *_this = static_cast<OpenGLWindow *>(glfwGetWindowUserPointer(window));
    _this->resizeEvent(width, height);
}

void OpenGLWindow::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto *_this = static_cast<OpenGLWindow *>(glfwGetWindowUserPointer(window));
    _this->keyEvent(key, scancode, action, mods);
}

void OpenGLWindow::mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto *_this = static_cast<OpenGLWindow *>(glfwGetWindowUserPointer(window));
    _this->mouseScrollEvent(xoffset, yoffset);
}

void OpenGLWindow::mouseMoveCallBack(GLFWwindow *window, double xpos, double ypos)
{
    auto *_this = static_cast<OpenGLWindow *>(glfwGetWindowUserPointer(window));
    _this->mouseMoveEvent(xpos, ypos);
}

void OpenGLWindow::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    auto *_this = static_cast<OpenGLWindow *>(glfwGetWindowUserPointer(window));
    _this->mouseButtonEvent(button, action, mods);
}

void OpenGLWindow::charCallback(GLFWwindow* window, unsigned int codepoint)
{
    auto *_this = static_cast<OpenGLWindow *>(glfwGetWindowUserPointer(window));
    _this->charEvent(codepoint);
}

void OpenGLWindow::initializeWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, m_profile);
    glfwWindowHint(GLFW_SAMPLES, m_samples);

    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (m_window == nullptr)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwSetWindowUserPointer(m_window, this);

    glfwSetWindowCloseCallback(m_window, closeCallback);
    glfwSetWindowFocusCallback(m_window, focusCallback);
    glfwSetWindowIconifyCallback(m_window, minimizeCallback);
    glfwSetWindowPosCallback(m_window, posCallback);
    glfwSetWindowRefreshCallback(m_window, updateCallback);
    glfwSetWindowSizeCallback(m_window, resizeCallback);

    glfwSetKeyCallback(m_window, keyCallback);
    glfwSetScrollCallback(m_window, mouseScrollCallback);
    glfwSetCursorPosCallback(m_window, mouseMoveCallBack);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
    glfwSetCharCallback(m_window, charCallback);

    glfwMakeContextCurrent(m_window);
    if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
    {
        std::cerr << "Failed to Initliaze GLAD" << std::endl;
        close();
    }
}

glm::vec2 OpenGLWindow::getCursorPos() const
{
    double xpos, ypos;
    glfwGetCursorPos(m_window, &xpos, &ypos);

    return {xpos, ypos};
}


void OpenGLWindow::setOpenGLContextVersion(int major, int minor)
{
    m_major = major;
    m_minor = minor;
}

void OpenGLWindow::setOpenGLProfile(OpenGLProfile profile)
{
    m_profile = profile;
}

void OpenGLWindow::setSamples(int samples)
{
    m_samples = samples;
}

void OpenGLWindow::setWindowTitle(const string &title)
{
    m_title = title;
    std::ostringstream sout;
    sout << m_title << "---FPS : " << m_fps;
    glfwSetWindowTitle(m_window, sout.str().c_str());
}

void OpenGLWindow::setWindowSize(int width, int height)
{
    glfwSetWindowSize(m_window, width, height);
}

void OpenGLWindow::setCursorMode(CursorMode mode)
{
    glfwSetInputMode(m_window, GLFW_CURSOR, mode);
}

void OpenGLWindow::exec()
{
#ifdef _WIN32
    m_lastTime = ::GetTickCount64();
#else
    m_lastTime = glfwGetTime();
#endif
    while (!glfwWindowShouldClose(m_window))
    {
        static unsigned int frame = 0;
        frame++;

#ifdef _WIN32
        auto now = ::GetTickCount64();
#else
        auto now = glfwGetTime();
#endif
        auto deltaTime = now - m_lastTime;
        m_lastTime = now;

        if (deltaTime >= 1000)
        {
            m_fps = frame;
            frame = 0;
        }
        setWindowTitle(m_title);

        render((float)deltaTime / 1000.0f);
        glfwSwapBuffers(m_window);

        glfwPollEvents();
    }

    glfwTerminate();
}

void OpenGLWindow::close()
{
    glfwSetWindowShouldClose(m_window, true);
}

