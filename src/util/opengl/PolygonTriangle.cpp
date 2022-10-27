//
// Created by co on 2022/10/25.
//

#include "PolygonTriangle.h"
#include <sstream>

PolygonTriangle::PolygonTriangle(const vector<Mesh>& meshes) : Polygon(meshes) {

}

void PolygonTriangle::addIndices(int meshIndex, unsigned int index0, unsigned int index1, unsigned int index2) {
    m_meshes[meshIndex].indices.push_back(index0);
    m_meshes[meshIndex].indices.push_back(index1);
    m_meshes[meshIndex].indices.push_back(index2);
}

void PolygonTriangle::removeIndices(int meshIndex, unsigned int index0, unsigned int index1, unsigned int index2) {
    for (int i = 0; i < m_meshes[meshIndex].indices.size(); i += 3) {
        if (m_meshes[meshIndex].indices[i] == index0 && m_meshes[meshIndex].indices[i + 1] == index1 && m_meshes[meshIndex].indices[i + 2] == index2) {
            m_meshes[meshIndex].indices.erase(m_meshes[meshIndex].indices.begin() + i, m_meshes[meshIndex].indices.begin() + i + 3);
            break;
        }
    }
}

void PolygonTriangle::draw(const PolygonMesh &mesh) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    for (int i = 0; i != mesh.indices.size(); i += 3)
        glDrawArrays(GL_TRIANGLES, mesh.indices[i], 3);
}

bool PolygonTriangle::in(int meshIndex, unsigned int index0, unsigned int index1, unsigned int index2) {
    for (int i = 0; i < m_meshes[meshIndex].indices.size(); i += 3) {
        if (m_meshes[meshIndex].indices[i] == index0 && m_meshes[meshIndex].indices[i + 1] == index1 && m_meshes[meshIndex].indices[i + 2] == index2)
            return true;
    }
    return false;
}

PolygonTriangle::PolygonTriangle() {

}

void PolygonTriangle::resetIndices(int meshIndex, unsigned int index0, unsigned int index1, unsigned int index2) {
    for (auto &mesh : m_meshes) {
        mesh.indices.clear();
    }
    addIndices(meshIndex, index0, index1, index2);
}

std::string PolygonTriangle::getIndicesString() const &{
    std::stringstream ss;
    for (const auto & mesh : m_meshes) {
        for (int j = 0; j < mesh.indices.size(); j += 3) {
            ss << "Triangle (" << mesh.indices[j] << ", " << mesh.indices[j + 1] << ", " << mesh.indices[j + 2] << ") " << std::endl;
        }
    }
    return ss.str();
}
