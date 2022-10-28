//
// Created by co on 2022/10/28.
//

#ifndef MODEL_VIEWER_LIGHTFACTORY_H
#define MODEL_VIEWER_LIGHTFACTORY_H

#define MAX_LIGHT_NUM 10

#include "opengl/Light.h"

class LightFactory {
public:
    ~LightFactory();

    static LightFactory &get() {
        static LightFactory instance;
        return instance;
    }

    LightFactory(LightFactory const &) = delete;
    void operator=(LightFactory const &) = delete;

    [[nodiscard]] Light *getLight(int index) const;

    int addLight(LightType type, int index = -1);

    void deleteLight(int index);

    void setBaseModel(Model *model);

    void importShaderValue(ShaderProgram &shaderProgram) const;

    void modelRender(ShaderProgram &shaderProgram, glm::mat4 &view, glm::mat4 &projection) const;

    void reset();

    void refreshTorchLightPos(glm::vec3 pos, glm::vec3 direction);
private:
    LightFactory();

    Model *baseModel;

    Light *lights[MAX_LIGHT_NUM];
};


#endif //MODEL_VIEWER_LIGHTFACTORY_H
