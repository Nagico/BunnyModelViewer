//
// Created by co on 2022/10/28.
//

#include "Light.h"

Light::Light(LightType type) {
    this->type = type;
}

void Light::importShaderValue(ShaderProgram &shaderProgram, int index) const {
    shaderProgram.use();
    string lightStr = "lights[" + std::to_string(index) + "].";
    shaderProgram.setValue(lightStr + "type", static_cast<int>(type));
    shaderProgram.setValue(lightStr + "color", color);
    shaderProgram.setValue(lightStr + "ambient", ambient);
    shaderProgram.setValue(lightStr + "diffuse", diffuse);
    shaderProgram.setValue(lightStr + "specular", specular);

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
    if (isShowModel && model != nullptr && modelMatrix != nullptr) {
        shaderProgram.use();
        shaderProgram.setValue("model", *modelMatrix);
        model->render(&shaderProgram);
    }
}

Light::Light(LightType type, Model *model, glm::mat4 *modelMatrix) {
    this->type = type;
    this->model = model;
    this->modelMatrix = modelMatrix;

}

Light::Light() {
    type = NONE;
}
