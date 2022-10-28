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
    SPOT_LIGHT
};

class Light {
public:
    Light();
    Light(LightType type);
    Light(LightType type, Model *model, glm::mat4 *modelMatrix);
    ~Light() = default;

    void importShaderValue(ShaderProgram &shaderProgram, int index) const;

    void modelRender(ShaderProgram &shaderProgram) const;

    LightType type;
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;

    Model *model;
    glm::mat4 *modelMatrix;
    bool isShowModel = true;
};


#endif //MODEL_VIEWER_LIGHT_H
