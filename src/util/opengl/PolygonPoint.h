//
// Created by co on 2022/10/25.
//

#ifndef POLYGONPOINT_H
#define POLYGONPOINT_H


#include "ShaderProgram.h"
#include "Polygon.h"

class PolygonPoint : public Polygon {
public:
    PolygonPoint();
    PolygonPoint(unsigned int vao);
    void addIndices(unsigned int index0, unsigned int index1 = 0, unsigned int index2 = 0) override;
    void removeIndices(unsigned int index0, unsigned int index1 = 0, unsigned int index2 = 0);

    void resetIndices(unsigned int index0, unsigned int index1 = 0, unsigned int index2 = 0) override;
private:
    void draw() override;
    bool in(unsigned int index0, unsigned int index1 = 0, unsigned int index2 = 0) override;
};


#endif //POLYGONPOINT_H
