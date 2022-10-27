#ifndef RAYPICKER_H
#define RAYPICKER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "opengl/Mesh.h"

class RayPicker {
public:
    RayPicker();

    static constexpr float POINT_PICK_EPSILON = 0.02f;

    glm::vec3 orig, dir;
    glm::vec3 crossPoint;

    int selectMeshIndex;
    bool selectFaceValid = false;
    VertexData selectFace[3];
    unsigned int selectFaceIndex[3];
    bool selectPointValid = false;
    VertexData selectPoint;
    unsigned int selectPointIndex;

    /// 射线拾取
    /// \param mesh 待查找面
    /// \param cameraPos 摄像机位置
    /// \param model 模型矩阵
    /// \param view 视图矩阵
    /// \param projection 投影矩阵
    /// \param xpos 鼠标x坐标
    /// \param ypos 鼠标y坐标
    /// \param width 窗口宽度
    /// \param height 窗口高度
    void rayPick(const vector<Mesh>& meshes, glm::vec3 cameraPos, const glm::mat4 &model, const glm::mat4 &view,
                 const glm::mat4 &projection, float xpos, float ypos, int width, int height);

private:
    float m_xpos, m_ypos;
    int m_width, m_height;
    float m_distance;

    glm::mat4 m_model, m_view, m_projection;

    std::vector<Mesh> m_meshes;

    /// 射线三角形检测 直接计算
    bool intersectTriangleBF(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2,
                             float& distanceOrig, float& parameterU, float& parameterV);

    static float getArea(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);

    /// 射线三角形检测
    /// \param orig 射线起点
    /// \param dir 射线方向
    /// \param v0 三角形顶点0
    /// \param v1 三角形顶点1
    /// \param v2 三角形顶点2
    /// \param t 交点距离起点的距离
    /// \param u 交点距离v0的距离
    /// \param v 交点距离v1的距离
    /// \return 是否相交
    /// 根据上面代码求出的t,u,v的值，交点的最终坐标可以用下面两种方法计算
    /// orig + dir * t
    /// (1 - u - v) * v0 + u * v1 + v * v2Model
    bool intersectTriangle(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2,
                           float& distanceOrig, float& parameterU, float& parameterV);

    void rayDirection();

    void checkSelectPoint();

    void checkFaces();
};


#endif
