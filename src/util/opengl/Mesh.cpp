#include "Mesh.h"
#include "ShaderProgram.h"
#include "glad/glad.h"
#include <iostream>
#include <utility>

Mesh::Mesh(const vector<VertexData> &vertices,
           const vector<unsigned int> &indices,
           const vector<Face> &faces,
           const vector<Texture> &textures,
           const MeshInfo &meshInfo) :
        m_vertices(vertices),
        m_indices(indices),
        m_faces(faces),
        m_textures(textures),
        m_meshInfo(meshInfo)
{
    setupMesh();
}


Mesh::~Mesh()
{
}

void Mesh::render(ShaderProgram *program, bool forceColor, bool useMeshInfo, unsigned int depthMap)
{
    size_t diffuseNum = 1;
    size_t specularNum = 1;
    size_t normalNum = 1;
    size_t heightNum = 1;
    size_t size = m_textures.size();

    if (size == 0 && !forceColor)
    {
        program->setValue("hasTexture", false);
        program->setValue("modelColor", glm::vec3(0.8f));
    }
    else
    {
        program->setValue("hasTexture", true);
    }

    for (size_t i = 0; i < size; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);  // 在绑定之前激活相应的纹理单元
        // 获取纹理序号（diffuse_textureN 中的 N）
        glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
        string name = m_textures[i].name;
        if (name == "diffuse")  // 漫反射贴图
            name += std::to_string(diffuseNum++);
        else if (name == "specular")  // 镜面贴图
            name += std::to_string(specularNum++);
        else if (name == "normal")  // 法线贴图
            name += std::to_string(normalNum++);
        else if (name == "height")  // 高度贴图
            name += std::to_string(heightNum++);

        program->setValue(("material." + name).c_str(), (int)i);
    }

    if (depthMap != -1) {
        glActiveTexture(GL_TEXTURE0 + size);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        program->setValue("shadowMap", (int)size);
    }

    glBindVertexArray(m_vao);  // 绑定VAO
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);  // 绘制网格
    glBindVertexArray(0);  // 解绑VAO
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(VertexData), &m_vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

    // 顶点位置
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void *)0);
    // 顶点法线
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void *)offsetof(VertexData, normal));
    // 顶点纹理坐标
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void *)offsetof(VertexData, texCoord));
    // 顶点切线
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, tangent));
    // 顶点副切线
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, bitangent));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

const vector<VertexData> &Mesh::getVertices() const {
    return m_vertices;
}

const vector<unsigned int> &Mesh::getIndices() const {
    return m_indices;
}

const vector<Texture> &Mesh::getTextures() const {
    return m_textures;
}

const MeshInfo &Mesh::getMeshInfo() const {
    return m_meshInfo;
}

const vector<Face> &Mesh::getFaces() const {
    return m_faces;
}

Mesh::Mesh() {

}

unsigned int Mesh::getVao() const {
    return m_vao;
}
