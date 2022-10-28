//
// Created by co on 2022/10/28.
//

#include "Light.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

Light::Light(LightType type) {
    this->type = type;
}

void Light::importShaderValue(ShaderProgram &shaderProgram, int index) const {
    shaderProgram.use();
    string lightStr = "lights[" + std::to_string(index) + "].";
    shaderProgram.setValue(lightStr + "type", static_cast<int>(type));
    shaderProgram.setValue(lightStr + "color", color);
    shaderProgram.setValue(lightStr + "ambient", glm::vec3(ambientX));
    shaderProgram.setValue(lightStr + "diffuse", glm::vec3(diffuseX));
    shaderProgram.setValue(lightStr + "specular", glm::vec3(specularX));
    auto cutOff = glm::cos(glm::radians(cutOffDegree));
    auto outerCutOff = glm::cos(glm::radians(outerCutOffDegree));

    switch (type) {
        case DIRECTIONAL_LIGHT:
            shaderProgram.setValue(lightStr + "direction", direction);
            break;
        case SPOT_LIGHT:
            shaderProgram.setValue(lightStr + "direction", direction);
            shaderProgram.setValue(lightStr + "cutOff", cutOff);
            shaderProgram.setValue(lightStr + "outerCutOff", outerCutOff);
            // fall through
        case POINT_LIGHT:
            shaderProgram.setValue(lightStr + "position", position);
            shaderProgram.setValue(lightStr + "constant", constant);
            shaderProgram.setValue(lightStr + "linear", linear);
            shaderProgram.setValue(lightStr + "quadratic", quadratic);
            break;
        default:
            break;

    }
}

void Light::modelRender(ShaderProgram &shaderProgram) const {
    if (isShowModel && model != nullptr) {
        shaderProgram.setValue("lightColor", color);
        model->render(&shaderProgram);
    }
}

Light::Light() {
    type = NONE;
}

void Light::reset() {
    // 初始化灯
    if(type == DIRECTIONAL_LIGHT) {
        auto lampPos = glm::vec3(5.f, 10.f, 10.f);
        auto lampModelMatrix = glm::translate(glm::mat4(1.f), lampPos);
        lampModelMatrix = glm::scale(lampModelMatrix, glm::vec3(.3f, .3f, .3f));
        modelMatrix = lampModelMatrix;
        direction = -lampPos;
        color = glm::vec3(1.f, 1.f, 1.f);
        ambientX = 0.085f;
        diffuseX = 0.28f;
        specularX = 1.0f;
        isShowModel = true;
    }

    if (type == POINT_LIGHT) {
        auto lampPos = glm::vec3(3.f, 8.f, 5.f);
        auto lampModelMatrix = glm::translate(glm::mat4(1.f), lampPos);
        lampModelMatrix = glm::scale(lampModelMatrix, glm::vec3(.3f, .3f, .3f));
        modelMatrix = lampModelMatrix;
        position = lampPos;
        direction = -lampPos;
        color = glm::vec3(1.f, 1.f, 1.f);
        ambientX = 0.3f;
        diffuseX = 0.5f;
        specularX = 1.0f;
        constant = 1.0f;
        linear = 0.027f;
        quadratic = 0.0028f;
        isShowModel = true;
    }

    if (type == SPOT_LIGHT) {
        auto lampPos = glm::vec3(1.f, 1.f, 1.f);
        auto lampModelMatrix = glm::translate(glm::mat4(1.f), lampPos);
        lampModelMatrix = glm::scale(lampModelMatrix, glm::vec3(.3f, .3f, .3f));
        modelMatrix = lampModelMatrix;
        position = lampPos;
        direction = glm::vec3(-0.844f, -1.649f, -0.769);
        color = glm::vec3(0.911f, 0.433f, 0.151f);
        ambientX = 0.0f;
        diffuseX = 0.68f;
        specularX = 1.0f;
        constant = 1.0f;
        linear = 0.027f;
        quadratic = 0.0028f;
        cutOffDegree = 13.309f;
        outerCutOffDegree = 17.64f;
        isShowModel = true;
    }

}
