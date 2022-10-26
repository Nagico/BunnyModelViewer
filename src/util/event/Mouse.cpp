//
// Created by co on 2022/10/25.
//

#include "Mouse.h"

#include <iostream>

glm::vec2 Mouse::position;
glm::vec2 Mouse::scroll;
std::map<MouseButton, bool> Mouse::state;
bool Mouse::m_firstMouse;
EventBus Mouse::m_EventBus;

void Mouse::init(GLFWwindow *window){
    m_firstMouse = true;
    position = glm::vec2(0.0f, 0.0f);

    m_EventBus = EventHandler::get().getEventBus();

    for(MouseButton i = MouseButton::LEFT; i <= MouseButton::LAST; i = static_cast<MouseButton>(static_cast<int>(i) + 1)){
        state[i] = false;
    }

    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, scrollCallback);
}

void Mouse::mouseButtonCallback(GLFWwindow *window, int buttonIn, int action, int mods) {
    auto button = toMouseButton(buttonIn);

    if (action == GLFW_PRESS) {
        state[button] = true;
        EventHandler::get().publishMouseClickHoldEvent(button, position);
    } else if (action == GLFW_RELEASE) {
        state[button] = false;
        EventHandler::get().publishMouseClickReleaseEvent(button, position);
    }
}

void Mouse::scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    auto offset = glm::vec2(xoffset, yoffset);
    scroll += offset;
    m_EventBus->postpone<event::Mouse::ScrollEvent>({offset, scroll});
}

void Mouse::cursorPosCallback(GLFWwindow *window, double xposIn, double yposIn) {
    auto newPos = glm::vec2(xposIn, yposIn);
    if (m_firstMouse) {
        position = newPos;
        m_firstMouse = false;
    }

    auto offset = newPos - position;
    offset.y = -offset.y;  // reverse y direction

    position = newPos;
    m_EventBus->postpone<event::Mouse::MoveEvent>({offset, position});
}

void Mouse::checkInLoop() {
    for (auto &i : state) {
        if (i.second) {
            EventHandler::get().publishMouseClickPressEvent(i.first, position);
        }
    }
}
