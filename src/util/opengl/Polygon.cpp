//
// Created by co on 2022/10/25.
//

#include "Polygon.h"

Polygon::Polygon(const vector<Mesh>& meshes) {
    for (auto &mesh : meshes) {
        PolygonMesh polygonMesh;
        polygonMesh.vao = mesh.getVao();
        polygonMesh.indices = vector<unsigned int>();
        polygonMesh.vertices = mesh.getVertices();

        m_meshes.push_back(polygonMesh);
    }
}

void Polygon::render(float offset, float size) {
    glPolygonOffset(offset, offset);
    glPointSize(size);

    for (auto &mesh : m_meshes) {
        glBindVertexArray(mesh.vao);
        draw(mesh);
    }

    glBindVertexArray(0);
    glDisable(GL_POLYGON_OFFSET_LINE);
    glPointSize(1.f);
}

bool Polygon::modifyIndices(int meshIndex, unsigned int index0, unsigned int index1, unsigned int index2) {
    if (in(meshIndex, index0, index1, index2)) {
        removeIndices(meshIndex, index0, index1, index2);
        return false;
    }
    else {
        addIndices(meshIndex, index0, index1, index2);
        return true;
    }
}

Polygon::Polygon() {

}


