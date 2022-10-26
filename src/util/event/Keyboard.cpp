//
// Created by co on 2022/10/26.
//

#include "Keyboard.h"

std::map<KeyboardKey, bool> Keyboard::state;
EventBus Keyboard::m_EventBus;

void Keyboard::init(GLFWwindow *window) {
    m_EventBus = EventHandler::get().getEventBus();

    for(KeyboardKey i = KeyboardKey::A; i <= KeyboardKey::LAST; i = static_cast<KeyboardKey>(static_cast<int>(i) + 1)){
        state[i] = false;
    }

    glfwSetKeyCallback(window, keyCallback);
}

void Keyboard::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    auto keyboardKey = toKeyboardKey(key);

    if (action == GLFW_PRESS) {
        state[keyboardKey] = true;
        EventHandler::get().publishKeyHoldEvent(keyboardKey);
    } else if (action == GLFW_RELEASE) {
        state[keyboardKey] = false;
        EventHandler::get().publishKeyReleaseEvent(keyboardKey);
    }
}

void Keyboard::checkInLoop() {
    for(auto &i : state){
        if(i.second){
            EventHandler::get().publishKeyPressEvent(i.first);
        }
    }
}
