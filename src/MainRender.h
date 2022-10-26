#ifndef MAINRENDER_H
#define MAINRENDER_H
#include "util/opengl/OpenGLRender.h"
#include "util/opengl/ShaderProgram.h"
#include "util/event/Event.h"
#include <glm/matrix.hpp>

class Model;
class Camera;
class Mouse;
class Keyboard;
class PolygonPoint;
class PolygonTriangle;
class RayPicker;

class MainRender : public OpenGLRender
{
public:
    struct Mode {
        bool lamp = true;
        bool fill = true;
        bool line = false;
        bool point = false;
        bool select = false;
    };

    MainRender(GLFWwindow *window, Camera *camera, Mouse *mouse, Keyboard *keyboard);
    ~MainRender();

    void render(float deltaTime) override;
    void resizeGL(int w, int h) override;
    void loadModel(const string &path);
    void unloadModel();

    bool modelLoaded;
    string modelName;

    RayPicker *rayPicker;

    Mode mode;
private:
    static constexpr float NEAR_PLANE = 0.1f;
    static constexpr float FAR_PLANE = 1000.f;

    GLFWwindow *m_window;
    int m_width, m_height;
    float m_deltaTime;

    Model *m_model;
    Model *m_lampModel;
    PolygonPoint *m_highlightPoint;
    PolygonPoint *m_selectPoint;
    PolygonTriangle *m_highlightTriangle;
    PolygonTriangle *m_selectTriangle;

    ShaderProgram m_modelShader, m_modelColorShader;
    ShaderProgram m_lampShader;

    glm::mat4 m_modelMatrix;
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;

    glm::mat4 m_lampModelMatrix;

    Camera *m_camera;

    Mouse *m_mouse;
    Keyboard *m_keyboard;

    glm::vec3 *m_lampPos;

    const glm::vec4 *m_lineColor, *m_pointColor, *m_selectPointColor,
            *m_selectTriangleColor, *m_highlightPointColor, *m_highlightTriangleColor;

    void initializeGL() override;
    void initializeShader() override;
    void initializeModel();
    void initializeRayPicker();

    void initializeEvent();

    void initializeRayPickerEvent(EventHandler &handler);

    void initializeCameraEvent(EventHandler &handler);

    void initializeModeChangeEvent(EventHandler &handler);

    void renderHighlight();

    void renderSelect();

    void renderFill();

    void renderLine();

    void renderPoint();

    void renderLamp();
};

#endif