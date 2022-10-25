//
// Created by co on 2022/10/25.
//

#include "PolygonPoint.h"

PolygonPoint::PolygonPoint(unsigned int vao) : Polygon(vao) {

}

void PolygonPoint::addIndices(unsigned int index0, unsigned int index1, unsigned int index2) {
    m_indices.push_back(index0);
}

void PolygonPoint::removeIndices(unsigned int index0, unsigned int index1, unsigned int index2) {
    m_indices.erase(std::remove(m_indices.begin(), m_indices.end(), index0), m_indices.end());
}

void PolygonPoint::draw() {
    for(auto& it : m_indices)
        glDrawArrays(GL_POINTS, it, 1);
}

bool PolygonPoint::in(unsigned int index0, unsigned int index1, unsigned int index2) {
    for (auto& it : m_indices) {
        if (it == index0)
            return true;
    }
    return false;
}

PolygonPoint::PolygonPoint() {

}

void PolygonPoint::resetIndices(unsigned int index0, unsigned int index1, unsigned int index2) {
    m_indices.clear();
    m_indices.push_back(index0);
}




