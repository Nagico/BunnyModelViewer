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
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

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

    GLint flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
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
    sout << m_title << " | FPS: " << m_fps;
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

        if (now - m_lastFPSTime >= 1000)
        {
            m_fps = frame;
            frame = 0;
            m_lastFPSTime = now;
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

void APIENTRY OpenGLWindow::glDebugOutput(GLenum source,
                                          GLenum type,
                                          GLuint id,
                                          GLenum severity,
                                          GLsizei length,
                                          const GLchar *message,
                                          const void *userParam)
{
    // 忽略一些不重要的错误/警告代码
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " <<  message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}
