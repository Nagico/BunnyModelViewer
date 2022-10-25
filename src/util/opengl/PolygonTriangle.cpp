//
// Created by co on 2022/10/25.
//

#include "PolygonTriangle.h"

PolygonTriangle::PolygonTriangle(unsigned int vao) : Polygon(vao) {

}

void PolygonTriangle::addIndices(unsigned int index0, unsigned int index1, unsigned int index2) {
    m_indices.push_back(index0);
    m_indices.push_back(index1);
    m_indices.push_back(index2);
}

void PolygonTriangle::removeIndices(unsigned int index0, unsigned int index1, unsigned int index2) {
    for (int i = 0; i < m_indices.size(); i += 3) {
        if (m_indices[i] == index0 && m_indices[i + 1] == index1 && m_indices[i + 2] == index2) {
            m_indices.erase(m_indices.begin() + i, m_indices.begin() + i + 3);
            break;
        }
    }
}

void PolygonTriangle::draw() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    for (int i = 0; i != m_indices.size(); i += 3)
        glDrawArrays(GL_TRIANGLES, m_indices[i], 3);
}

bool PolygonTriangle::in(unsigned int index0, unsigned int index1, unsigned int index2) {
    for (int i = 0; i < m_indices.size(); i += 3) {
        if (m_indices[i] == index0 && m_indices[i + 1] == index1 && m_indices[i + 2] == index2)
            return true;
    }
    return false;
}

PolygonTriangle::PolygonTriangle() {

}

void PolygonTriangle::resetIndices(unsigned int index0, unsigned int index1, unsigned int index2) {
    m_indices.clear();
    m_indices.push_back(index0);
    m_indices.push_back(index1);
    m_indices.push_back(index2);
}
