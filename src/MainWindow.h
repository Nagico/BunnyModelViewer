#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "util/opengl/OpenGLWindow.h"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class Camera;
class MainRender;
class MainWindow : public OpenGLWindow
{
public:
    MainWindow();
    ~MainWindow();

protected:
    void render();
    void resizeEvent(int width, int height);
    void keyEvent(int key, int scancode, int action, int mods);
    void mouseMoveEvent(double xpos, double ypos);

private:
    Camera *m_camera;
    MainRender *m_render;
};

#endif