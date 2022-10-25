#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H
#include <string>
#include "glad/glad.h"
#include "glm/matrix.hpp"

using std::string;
class ShaderProgram
{
public:
    enum ShaderType
    {
        Vertex = GL_VERTEX_SHADER,
        Fragment = GL_FRAGMENT_SHADER,
        Geometry = GL_GEOMETRY_SHADER
    };

public:
    ShaderProgram();
    ShaderProgram(const string& vertex_path, const string& fragment_path, const string& geometry_path = "");
    ~ShaderProgram();

    GLuint programId() const { return m_programId; }
    string lastError() const { return m_lastError; }

    bool addShader(ShaderType type, const string &source);
    bool addShaderFile(ShaderType type, const string &filename);

    void load(const string& vertex_path, const string& fragment_path, const string& geometry_path = "");

    void use() const;
    void use(const glm::mat4 model, const glm::mat4 view, const glm::mat4 projection);
    bool link();
    void setValue(const string &name, int value) const;
    void setValue(const string &name, float value) const;
    void setValue(const string &name, float x, float y);
    void setValue(const string &name, float x, float y, float z);
    void setValue(const string &name, float x, float y, float z, float w);
    void setValue(const string &name, const glm::vec2 &value) const;
    void setValue(const string &name, const glm::vec3 &value) const;
    void setValue(const string &name, const glm::vec4 &value) const;
    void setValue(const string &name, const glm::mat2 &value) const;
    void setValue(const string &name, const glm::mat3 &value) const;
    void setValue(const string &name, const glm::mat4 &value) const;

private:
    GLuint compileShader(ShaderType type, const string &source);
    GLuint compileShaderFile(ShaderType type, const string &filename);

    string m_lastError;
    GLuint m_programId;
};

#endif