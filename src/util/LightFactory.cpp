﻿//
// Created by co on 2022/10/28.
//

#include "LightFactory.h"

Light *LightFactory::getLight(int index) const {
    if (index < 0 || index >= MAX_LIGHT_NUM) {
        return nullptr;
    }
    return lights[index];
}

int LightFactory::addLight(LightType type, int index) {
    if (index >= 0 && index < MAX_LIGHT_NUM) {  // 指定位置
        if (lights[index]->type != NONE) return -1;  // 位置已被占用
        lights[index]->type = type;
        return index;
    }
    for (int i = 0; i < MAX_LIGHT_NUM; ++i) {  // 未指定位置
        if (lights[i]->type == NONE) {  // 找到空位置
            lights[i]->type = type;
            return i;
        }
    }
    return -1;  // 没有空位置或者指定位置不合法
}

void LightFactory::deleteLight(int index) {
    if (index >= 0 && index < MAX_LIGHT_NUM) {
        if (lights[index]->type != NONE) {
            delete lights[index];
            lights[index]->type = NONE;
        }
    }
}

void LightFactory::importShaderValue(ShaderProgram &shaderProgram) const {
    for (int i = 0; i < MAX_LIGHT_NUM; ++i) {
        if (lights[i]->type != NONE) {
            lights[i]->importShaderValue(shaderProgram, i);
        }
    }
}

void LightFactory::modelRender(ShaderProgram &shaderProgram, glm::mat4 &view, glm::mat4 &projection) const {
    for (auto light : lights) {
        if (light->type != NONE) {
            shaderProgram.use();
            shaderProgram.setValue("view", view);
            shaderProgram.setValue("projection", projection);
            light->modelRender(shaderProgram);
        }
    }
}

LightFactory::LightFactory() {
    for (auto & light : lights) {
        light = new Light();
    }
}

LightFactory::~LightFactory() {
    for (auto & light : lights) {
        delete light;
    }
}



