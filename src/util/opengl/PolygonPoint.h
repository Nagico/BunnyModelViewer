//
// Created by co on 2022/10/25.
//

#ifndef POLYGONPOINT_H
#define POLYGONPOINT_H
#include <map>

#include "ShaderProgram.h"
#include "Polygon.h"

class PolygonPoint : public Polygon {
public:
    PolygonPoint();
    PolygonPoint(unsigned int vao, const vector<VertexData> &vertex);

    void setVertex(const vector<VertexData> &vertex);

    void addIndices(unsigned int index0, unsigned int index1 = 0, unsigned int index2 = 0) override;
    void removeIndices(unsigned int index0, unsigned int index1 = 0, unsigned int index2 = 0);

    bool modifyIndices(unsigned int index0, unsigned int index1 = 0, unsigned int index2 = 0);

    void resetIndices(unsigned int index0, unsigned int index1 = 0, unsigned int index2 = 0) override;
private:
    vector<VertexData> m_vertex;

    void draw() override;
    bool in(unsigned int index0, unsigned int index1 = 0, unsigned int index2 = 0) override;

    [[nodiscard]] unsigned int getIndex(const glm::vec3& pos) const;

    std::map<std::tuple<float, float, float>, unsigned int> m_points;
};


#endif //POLYGONPOINT_H
