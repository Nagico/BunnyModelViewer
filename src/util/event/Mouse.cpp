//
// Created by co on 2022/10/25.
//

#include "Mouse.h"

glm::vec2 Mouse::position;
glm::vec2 Mouse::scroll;
std::map<MouseButton, bool> Mouse::state;
Listener Mouse::listener;
bool Mouse::m_firstMouse;
EventBus Mouse::m_EventBus;

void Mouse::init(GLFWwindow *window){
    m_firstMouse = true;
    position = glm::vec2(0.0f, 0.0f);

    m_EventBus = EventHandler::get().getEventBus();
    listener = EventHandler::get().getListener();

    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, scrollCallback);
}

void Mouse::mouseButtonCallback(GLFWwindow *window, int buttonIn, int action, int mods) {
    auto button = static_cast<MouseButton>(buttonIn);

    if (action == GLFW_PRESS) {
        if (state[button])
            EventHandler::get().publishMouseClickHoldEvent(button, position);

        state[button] = true;
        EventHandler::get().publishMouseClickEvent(button, position);
    } else if (action == GLFW_RELEASE) {
        state[button] = false;
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