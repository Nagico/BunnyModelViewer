//
// Created by co on 2022/10/25.
//

#include "Polygon.h"

Polygon::Polygon(unsigned int vao) {
    m_vao = vao;
}

void Polygon::render(float offset, float size) {
    glPolygonOffset(offset, offset);
    glPointSize(size);

    glBindVertexArray(m_vao);
    draw();

    glBindVertexArray(0);
    glDisable(GL_POLYGON_OFFSET_LINE);
    glPointSize(1.f);
}

void Polygon::modifyIndices(unsigned int index0, unsigned int index1, unsigned int index2) {
    if (in(index0, index1, index2))
        removeIndices(index0, index1, index2);
    else
        addIndices(index0, index1, index2);
}

Polygon::Polygon() {
    m_vao = 0;
}

void Polygon::setVao(unsigned int vao) {
    m_vao = vao;
}


