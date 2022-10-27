//
// Created by co on 2022/10/25.
//

#include "PolygonPoint.h"
#include <sstream>

PolygonPoint::PolygonPoint(const vector<Mesh>& meshes) : Polygon(meshes) {
}

void PolygonPoint::addIndices(int meshIndex, unsigned int index0, unsigned int index1, unsigned int index2) {
    m_meshes[meshIndex].indices.push_back(index0);
    auto pos = std::make_tuple(m_meshes[meshIndex].vertices[index0].position.x,
                               m_meshes[meshIndex].vertices[index0].position.y,
                               m_meshes[meshIndex].vertices[index0].position.z);
    m_points.insert({pos, {index0, meshIndex}});
}

void PolygonPoint::removeIndices(int meshIndex, unsigned int index0, unsigned int index1, unsigned int index2) {
    // 需要处理相同坐标、不同ID的点

    auto info = getInfo(m_meshes[meshIndex].vertices[index0].position);  // 根据坐标在记录表中查找已绘制的点信息
    if (info.index != 0xffffffff) {  // 如果找到了
        // 从indices中删除查找到的点
        m_meshes[info.meshIndex].indices.erase(
                std::find(m_meshes[info.meshIndex].indices.begin(), m_meshes[info.meshIndex].indices.end(), info.index));
        // 从记录表中删除点坐标对应的信息
        m_points.erase({
            m_meshes[meshIndex].vertices[index0].position.x,
            m_meshes[meshIndex].vertices[index0].position.y,
            m_meshes[meshIndex].vertices[index0].position.z});
    }
}

void PolygonPoint::draw(const PolygonMesh &mesh) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    for(auto& it : mesh.indices)
        glDrawArrays(GL_POINTS, it, 1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

bool PolygonPoint::in(int meshIndex, unsigned int index0, unsigned int index1, unsigned int index2) {
    auto pointPos = m_meshes[meshIndex].vertices[index0].position;
    if (getInfo(pointPos).index != 0xffffffff)
        return true;
    return false;
}

PolygonPoint::PolygonPoint() {

}

void PolygonPoint::resetIndices(int meshIndex, unsigned int index0, unsigned int index1, unsigned int index2) {
    for (auto &mesh: m_meshes) {
        mesh.indices.clear();
    }
    m_points.clear();

    addIndices(meshIndex, index0, index1, index2);
}

PolygonPoint::PointInfo PolygonPoint::getInfo(const glm::vec3& pos) const {
    auto key = std::make_tuple(pos.x, pos.y, pos.z);
    auto index = m_points.find(key);
    if (index != m_points.end())
        return index->second;
    return {0xffffffff, -1};  // max unsigned int
}

bool PolygonPoint::modifyIndices(int meshIndex, unsigned int index0, unsigned int index1, unsigned int index2) {
    auto pointPos = m_meshes[meshIndex].vertices[index0].position;
    auto info = getInfo(pointPos);
    if (info.index != 0xffffffff)  // if the point is in the vector of points
    {
        m_meshes[meshIndex].indices.erase(std::find(m_meshes[meshIndex].indices.begin(), m_meshes[meshIndex].indices.end(), info.index));
        m_points.erase({m_meshes[meshIndex].vertices[index0].position.x, m_meshes[meshIndex].vertices[index0].position.y, m_meshes[meshIndex].vertices[index0].position.z});
        return false;
    }
    else
    {
        addIndices(meshIndex, index0, index1, index2);
        return true;
    }
}

std::string PolygonPoint::getIndicesString() const& {
    std::stringstream ss;
    for (auto &mesh: m_meshes) {
        for (auto &index: mesh.indices) {
            ss << "Point " << index << ": " <<
            mesh.vertices[index].position.x << ", " <<
            mesh.vertices[index].position.y << ", " <<
            mesh.vertices[index].position.z << std::endl;
        }
    }
    return ss.str();
}

