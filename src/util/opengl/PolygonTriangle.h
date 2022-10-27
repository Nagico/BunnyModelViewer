//
// Created by co on 2022/10/25.
//

#ifndef POLYGONTRIANGLE_H
#define POLYGONTRIANGLE_H


#include "ShaderProgram.h"
#include "Polygon.h"

class PolygonTriangle : public Polygon {
public:
    PolygonTriangle();

    explicit PolygonTriangle(const vector<Mesh>& meshes);
    void addIndices(int meshIndex, unsigned int index0, unsigned int index1, unsigned int index2) override;
    void removeIndices(int meshIndex, unsigned int index0, unsigned int index1, unsigned int index2) override;
    void resetIndices(int meshIndex, unsigned int index0, unsigned int index1, unsigned int index2) override;
private:
    void draw(const PolygonMesh &mesh) override;
    bool in(int meshIndex, unsigned int index0, unsigned int index1, unsigned int index2) override;
};



#endif //POLYGONTRIANGLE_H
