//
// Created by co on 2022/10/24.
//

#include <iostream>
#include "RayPicker.h"

void RayPicker::rayPick(const Mesh& mesh, const glm::vec3 cameraPos,
                        const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection,
                        float xpos, float ypos, int width, int height) {
    this->m_mesh = mesh;
    this->m_model = model;
    this->m_view = view;
    this->m_projection = projection;
    this->m_xpos = xpos;
    this->m_ypos = ypos;
    this->m_width = width;
    this->m_height = height;
    this->orig = cameraPos;

    rayDirection();
    checkFaces();
}

bool RayPicker::intersectTriangleBF(
                       const glm::vec3& v0,
                       const glm::vec3& v1,
                       const glm::vec3& v2,
                       float& distanceOrig,
                       float& parameterU,
                       float& parameterV)
{
    auto v0Model = glm::vec3(m_model * glm::vec4(v0, 1.0f));
    auto v1Model = glm::vec3(m_model * glm::vec4(v1, 1.0f));
    auto v2Model = glm::vec3(m_model * glm::vec4(v2, 1.0f));

    // Ax+By+Cz+D=0
    auto A = v0Model.y * (v2Model.z - v1Model.z) + v1Model.y * (v0Model.z - v2Model.z) + v2Model.y * (v1Model.z - v0Model.z);
    auto B = v0Model.z * (v2Model.x - v1Model.x) + v1Model.z * (v0Model.x - v2Model.x) + v2Model.z * (v1Model.x - v0Model.x);
    auto C = v0Model.x * (v2Model.y - v1Model.y) + v1Model.x * (v0Model.y - v2Model.y) + v2Model.x * (v1Model.y - v0Model.y);
    auto D = -(v0Model.x * (v2Model.y * v1Model.z - v1Model.y * v2Model.z) + v1Model.x * (v0Model.y * v2Model.z - v2Model.y * v0Model.z) + v2Model.x * (v1Model.y * v0Model.z - v0Model.y * v1Model.z));

    auto point1 = orig;
    auto point2 = orig + dir;

    // 线段与平面的交点
    distanceOrig = -(A * point1.x + B * point1.y + C * point1.z + D) / (A * (point2.x - point1.x) + B * (point2.y - point1.y) + C * (point2.z - point1.z));
    auto point = point1 + distanceOrig * (point2 - point1);

    if (distanceOrig < 0.001f)
    {
        return false;
    }

    // 判断交点是否在三角形内
    auto s012 = getArea(v0Model, v1Model, v2Model);
    auto s01p = getArea(v0Model, v1Model, point);
    auto s02p = getArea(v0Model, v2Model, point);
    auto s12p = getArea(v1Model, v2Model, point);

    auto s = s01p + s02p + s12p;
    auto res = (s - s012) < 0.001f;

    return res;
}

float RayPicker::getArea(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
{
    auto a = glm::length(v1 - v0);
    auto b = glm::length(v2 - v1);
    auto c = glm::length(v0 - v2);

    auto p = (a + b + c) / 2.0f;

    return sqrt(p * (p - a) * (p - b) * (p - c));
}

bool RayPicker::intersectTriangle(
                        const glm::vec3& v0,
                        const glm::vec3& v1,
                        const glm::vec3& v2,
                        float& distanceOrig,
                        float& parameterU,
                        float& parameterV)
{
    constexpr float EPSILON = 0.0000001f;

    auto v0Model = glm::vec3(m_model * glm::vec4(v0, 1.0f));
    auto v1Model = glm::vec3(m_model * glm::vec4(v1, 1.0f));
    auto v2Model = glm::vec3(m_model * glm::vec4(v2, 1.0f));

    // 计算三角形的法线
    auto E1 = v1Model - v0Model;  // 三角形的边
    auto E2 = v2Model - v0Model;
    auto P = glm::cross(dir, E2);

    auto det = glm::dot(E1, P);

    // 如果det=0，说明射线与三角形平行，不相交
    if (det > -EPSILON && det < EPSILON) {
        return false;
    }


    auto T = orig - v0Model;
    // 计算u
    parameterU = glm::dot(T, P);
    if (parameterU < EPSILON || parameterU - det > -EPSILON)
    {
        return false;
    }

    auto Q = glm::cross(T, E1);

    // 计算v
    parameterV = glm::dot(dir, Q);
    if (parameterV < EPSILON || parameterU + parameterV - det > -EPSILON)
    {
        return false;
    }

    // 计算t
    distanceOrig = glm::dot(E2, Q);

    auto fInvDet = 1.0f / det;
    distanceOrig *= fInvDet;
    parameterU *= fInvDet;
    parameterV *= fInvDet;

    return true;
}

void RayPicker::rayDirection()
{
    float x = (2.0f * m_xpos) / m_width - 1.0f;
    float y = 1.0f - (2.0f * m_ypos) / m_height;
    float z = 1.0f;  //  z = 1.0f 代表当前将鼠标的位置投影到远裁剪平面，如果设z的坐标为-1则代表将当前投影到近裁剪平面上
    auto ray_nds = glm::vec3(x, y, z);

    // 裁剪齐次坐标
    auto ray_clip = glm::vec4(ray_nds.x, ray_nds.y, ray_nds.z, 1.0f);

    // 视点坐标
    // 投影矩阵的逆矩阵左乘点的裁剪坐标
    auto ray_eye = glm::inverse(m_projection) * ray_clip;

    // 世界坐标
    auto ray_wor = glm::inverse(m_view) * ray_eye;

    if (ray_wor.w != 0)
    {
        ray_wor = ray_wor / ray_wor.w;
    }

    // 从摄像机位置发出一条射线
    dir = glm::normalize(glm::vec3(ray_wor.x, ray_wor.y, ray_wor.z) - orig);
}

void RayPicker::checkFaces()
{
    float minT = 1000.f;
    Face findFace;

    float u, v, t;


#pragma omp parallel
#pragma omp for
        for (auto& face : m_mesh.getFaces())
        {
            VertexData vertex[3];
            for (int i = 0; i < 3; i++)
                vertex[i] = m_mesh.getVertices()[face.vertex[i]];

            if (intersectTriangle(vertex[0].position, vertex[1].position, vertex[2].position, t, u, v))
            {
                if (t > 0 && t < minT)
                {
                    minT = t;
                    findFace = face;
                }
            }
        }

    if (minT < 1000.f)
    {
        selectFaceValid = true;
        for (int i = 0; i < 3; i++)
        {
            selectFace[i] = m_mesh.getVertices()[findFace.vertex[i]];
            selectFaceIndex[i] = findFace.vertex[i];
        }
        crossPoint = orig + dir * minT;
        m_distance = minT;
        checkSelectPoint();
    }
    else
    {
        selectPointValid = false;
        selectFaceValid = false;
    }
}

void RayPicker::checkSelectPoint()
{
    auto pointView = m_projection * m_view * glm::vec4(crossPoint, 1.0f);

    float len[3], minLen = 1000.f;
    int index = -1;

    for (int i = 0; i < 3; i++)
    {
        auto pointTriangleView = m_projection * m_view * m_model * glm::vec4(selectFace[i].position, 1.0f);
        len[i] = glm::length(glm::vec2(pointTriangleView.x, pointTriangleView.y) - glm::vec2(pointView.x, pointView.y));
        if (len[i] < minLen && len[i] <= POINT_PICK_EPSILON * m_distance)
        {
            minLen = len[i];
            index = i;
        }
    }

    if (index != -1)
    {
        selectPointValid = true;
        selectPoint = selectFace[index];
        selectPointIndex = selectFaceIndex[index];
    }
    else
    {
        selectPointValid = false;
    }
}

RayPicker::RayPicker() {

}
