#ifndef MAINRENDER_H
#define MAINRENDER_H
#include "util/opengl/OpenGLRender.h"
#include "util/opengl/ShaderProgram.h"
#include <glm/matrix.hpp>

class Model;
class Camera;
class MainRender : public OpenGLRender
{
public:
    MainRender(Camera *camera);
    ~MainRender();

    void render();
    void resizeGL(int w, int h);
    void initializeGL();
    void initializeShader();
    void initializeModel();
    void initializeLight();

private:
    GLuint m_lampVao, m_vbo, m_ebo;
    ShaderProgram m_modelProgram, m_lampProgram[6];
    glm::mat4x4 m_projection;
    Camera *m_camera;
    Model *m_model;
};

#endif