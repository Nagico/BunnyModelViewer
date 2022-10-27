#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "ShaderProgram.h"
#include "Mesh.h"

using std::vector;

class Polygon {
public:
    struct PolygonMesh {
        unsigned int vao;
        vector<unsigned int> indices;
        vector<VertexData> vertices;
    };

    Polygon();
    Polygon(const vector<Mesh>& meshes);

    void render(float offset, float size = 0.f);
    virtual void addIndices(int meshIndex, unsigned int index0, unsigned int index1 = 0, unsigned int index2 = 0) = 0;
    virtual void removeIndices(int meshIndex, unsigned int index0, unsigned int index1 = 0, unsigned int index2 = 0) = 0;
    bool modifyIndices(int meshIndex, unsigned int index0, unsigned int index1 = 0, unsigned int index2 = 0);

    virtual void resetIndices(int meshIndex, unsigned int index0, unsigned int index1 = 0, unsigned int index2 = 0) = 0;


protected:
    vector<PolygonMesh> m_meshes;

    virtual void draw(const PolygonMesh &mesh) = 0;
    virtual bool in(int meshIndex, unsigned int index0, unsigned int index1 = 0, unsigned int index2 = 0) = 0;
};


#endif //POLYGON_H
