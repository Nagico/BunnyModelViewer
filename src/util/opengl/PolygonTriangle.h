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

    PolygonTriangle(unsigned int vao);
    void addIndices(unsigned int index0, unsigned int index1, unsigned int index2) override;
    void removeIndices(unsigned int index0, unsigned int index1, unsigned int index2) override;
    void resetIndices(unsigned int index0, unsigned int index1, unsigned int index2) override;
private:
    void draw() override;
    bool in(unsigned int index0, unsigned int index1, unsigned int index2) override;
};



#endif //POLYGONTRIANGLE_H
