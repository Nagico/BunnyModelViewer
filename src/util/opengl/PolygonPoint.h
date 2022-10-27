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
    PolygonPoint(const vector<Mesh>& meshes);

    void addIndices(int meshIndex, unsigned int index0, unsigned int index1 = 0, unsigned int index2 = 0) override;
    void removeIndices(int meshIndex, unsigned int index0, unsigned int index1 = 0, unsigned int index2 = 0);

    bool modifyIndices(int meshIndex, unsigned int index0, unsigned int index1 = 0, unsigned int index2 = 0);

    void resetIndices(int meshIndex, unsigned int index0, unsigned int index1 = 0, unsigned int index2 = 0) override;
private:
    struct PointInfo {
        unsigned int index;
        int meshIndex;
    };

    void draw(const PolygonMesh &mesh) override;
    bool in(int meshIndex, unsigned int index0, unsigned int index1 = 0, unsigned int index2 = 0) override;

    [[nodiscard]] PointInfo getInfo(const glm::vec3& pos) const;

    /// key: point position <x, y, z>
    /// value: point info
    std::map<std::tuple<float, float, float>, PointInfo> m_points;
};


#endif //POLYGONPOINT_H
