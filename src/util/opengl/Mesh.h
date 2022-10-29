#ifndef OPENGLMESH_H
#define OPENGLMESH_H

#include <string>
#include <vector>
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/glm.hpp"

using std::string;
using std::vector;
struct VertexData
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

struct Texture
{
    unsigned int id;
    string name;
    string path;  // 储存纹理的路径用于与其它纹理进行比较
};

struct Face
{
    unsigned int vertex[3];
};

struct MeshInfo
{
    bool valid = false;

    float shininess = 32.0f;  // 材质的高光指数
    float dissolve = 1.0f;  // 材质的透明度
    float refractiveIndex = 1.0f;  // 材质的折射率

    int illum = 3;  // 材质的光照模型

    glm::vec3 ambient = glm::vec3(1);  // 材质的环境光反射系数
    glm::vec3 diffuse = glm::vec3(1);  // 材质的漫反射系数
    glm::vec3 specular = glm::vec3(1);  // 材质的镜面反射系数
    glm::vec3 emission = glm::vec3(1);  // 材质的自发光系数

    glm::vec3 maxVertex = glm::vec3(1.f);  // 最大位置分量
    glm::vec3 minVertex = glm::vec3(1.f);  // 最小位置分量

};

class ShaderProgram;
class Mesh
{
public:
    Mesh(const vector<VertexData> &vertices,
         const vector<unsigned int> &indices,
         const vector<Face> &faces,
         const vector<Texture> &textures,
         const MeshInfo &meshInfo);

    Mesh();

    ~Mesh();


    void render(ShaderProgram *program,bool forceColor=false, bool useMeshInfo = true, unsigned int depthMap = -1);

    [[nodiscard]] unsigned int getVao() const;

    [[nodiscard]] const vector<VertexData> &getVertices() const;
    [[nodiscard]] const vector<unsigned int> &getIndices() const;
    [[nodiscard]] const vector<Face> &getFaces() const;
    [[nodiscard]] const vector<Texture> &getTextures() const;
    [[nodiscard]] const MeshInfo &getMeshInfo() const;

private:
    void setupMesh();

    unsigned int m_vao, m_vbo, m_ebo;
    vector<VertexData> m_vertices;
    vector<unsigned int> m_indices;
    vector<Texture> m_textures;
    vector<Face> m_faces;
    MeshInfo m_meshInfo;
};

#endif