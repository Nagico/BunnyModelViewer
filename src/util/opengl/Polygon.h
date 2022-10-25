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
    Polygon();
    Polygon(unsigned vao);

    void setVao(unsigned vao);
    void render(float offset, float size = 0.f);
    virtual void addIndices(unsigned int index0, unsigned int index1 = 0, unsigned int index2 = 0) = 0;
    virtual void removeIndices(unsigned int index0, unsigned int index1 = 0, unsigned int index2 = 0) = 0;
    void modifyIndices(unsigned int index0, unsigned int index1 = 0, unsigned int index2 = 0);

    virtual void resetIndices(unsigned int index0, unsigned int index1 = 0, unsigned int index2 = 0) = 0;

    vector<unsigned int> m_indices;
protected:
    unsigned int m_vao;

    virtual void draw() = 0;
    virtual bool in(unsigned int index0, unsigned int index1 = 0, unsigned int index2 = 0) = 0;
};


#endif //POLYGON_H
