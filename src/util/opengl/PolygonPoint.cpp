//
// Created by co on 2022/10/25.
//

#include "PolygonPoint.h"

PolygonPoint::PolygonPoint(unsigned int vao) : Polygon(vao) {

}

void PolygonPoint::addIndices(unsigned int index0, unsigned int index1, unsigned int index2) {
    m_indices.push_back(index0);
    m_points.insert({{m_vertex[index0].position.x, m_vertex[index0].position.y, m_vertex[index0].position.z}, index0});
}

void PolygonPoint::removeIndices(unsigned int index0, unsigned int index1, unsigned int index2) {
    auto index = getIndex(m_vertex[index0].position);
    if (index != 0xffffffff) {
        m_indices.erase(std::find(m_indices.begin(), m_indices.end(), index));
        m_points.erase({m_vertex[index0].position.x, m_vertex[index0].position.y, m_vertex[index0].position.z});
    }
}

void PolygonPoint::draw() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    for(auto& it : m_indices)
        glDrawArrays(GL_POINTS, it, 1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

bool PolygonPoint::in(unsigned int index0, unsigned int index1, unsigned int index2) {
    auto pointPos = m_vertex[index0].position;
    if (getIndex(pointPos) != 0xffffffff)
        return true;
    return false;
}

PolygonPoint::PolygonPoint() {

}

void PolygonPoint::resetIndices(unsigned int index0, unsigned int index1, unsigned int index2) {
    m_indices.clear();
    m_indices.push_back(index0);
    m_points.clear();
    m_points.insert({{m_vertex[index0].position.x, m_vertex[index0].position.y, m_vertex[index0].position.z}, index0});
}

unsigned int PolygonPoint::getIndex(const glm::vec3& pos) const {
    auto key = std::make_tuple(pos.x, pos.y, pos.z);
    auto index = m_points.find(key);
    if (index != m_points.end())
        return index->second;
    return 0xffffffff;  // max unsigned int
}

void PolygonPoint::modifyIndices(unsigned int index0, unsigned int index1, unsigned int index2) {
    auto pointPos = m_vertex[index0].position;
    auto index = getIndex(pointPos);
    if (index != 0xffffffff)  // if the point is in the vector of points
    {
        m_indices.erase(std::find(m_indices.begin(), m_indices.end(), index));
        m_points.erase({m_vertex[index0].position.x, m_vertex[index0].position.y, m_vertex[index0].position.z});
    }
    else
    {
        addIndices(index0, index1, index2);
    }
}

void PolygonPoint::setVertex(const vector<VertexData> &vertex) {
    m_vertex = vertex;
}
