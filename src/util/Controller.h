//
// Created by co on 2022/10/29.
//

#ifndef MODEL_VIEWER_CONTROLLER_H
#define MODEL_VIEWER_CONTROLLER_H

#include <string>

class MainRender;
class Mouse;
class Camera;
struct GLFWwindow;

class Controller {
public:
    Controller(MainRender *render, Mouse *mouse, Camera *camera);
    ~Controller();

    void initialize(GLFWwindow *window);
    void prepareRender();
    void render();

private:
    MainRender *m_render;
    Mouse *m_mouse;
    Camera *m_camera;

    [[nodiscard]] std::string openFile() const&;

    void showModelTab() const;
    void showCameraTab() const;
    void showLightTab();
    void showSelectTab() const;
    void showHighlightTab() const;
};


#endif //MODEL_VIEWER_CONTROLLER_H
