#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "util/opengl/OpenGLWindow.h"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class Camera;
class MainRender;
class Mouse;
class Keyboard;

class MainWindow : public OpenGLWindow
{
public:
    MainWindow(int width = 800, int height = 600);
    ~MainWindow() override;

    bool loadModel(const string &path);
    bool unloadModel();

protected:
    void render(float deltaTime) override;
    void resizeEvent(int width, int height) override;
    void keyEvent(int key, int scancode, int action, int mods) override;
    void mouseMoveEvent(double xpos, double ypos) override;
    void mouseScrollEvent(double xoffset, double yoffset) override;
    void mouseButtonEvent(int button, int action, int mods) override;

private:
    Camera *m_camera;
    Mouse *m_mouse;
    Keyboard *m_keyboard;

    MainRender *m_render;

    void initializeEvent();

    void refreshTitle();
};

#endif