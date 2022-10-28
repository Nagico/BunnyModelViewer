//
// Created by co on 2022/10/28.
//

#ifndef MODEL_VIEWER_LIGHT_H
#define MODEL_VIEWER_LIGHT_H
#include "glm/matrix.hpp"
#include "ShaderProgram.h"
#include "Model.h"

enum LightType {
    NONE,
    DIRECTIONAL_LIGHT,
    POINT_LIGHT,
    SPOT_LIGHT,
    TORCH_LIGHT
};

class Light {
public:
    Light();
    explicit Light(LightType type);
    ~Light() = default;

    void importShaderValue(ShaderProgram &shaderProgram, int index) const;

    void modelRender(ShaderProgram &shaderProgram) const;

    void reset();

    LightType type = NONE;
    glm::vec3 position = glm::vec3(3.f, 8.f, 5.f);
    glm::vec3 direction = -glm::vec3(3.f, 8.f, 5.f);
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

    float ambientX = 0.6f;
    float diffuseX = 0.8f;
    float specularX = 1.0f;

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

    float cutOffDegree = 12.5f;
    float outerCutOffDegree = 17.5f;

    Model *model = nullptr;
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    bool isShowModel = true;
};


#endif //MODEL_VIEWER_LIGHT_H
