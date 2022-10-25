#ifndef MOUSE_H
#define MOUSE_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLFW/glfw3.h"

#include "Event.h"


class Mouse {
public:
    static void init(GLFWwindow *window);

    static glm::vec2 position;
    static glm::vec2 scroll;
    static std::map<MouseButton, bool> state;
    static Listener listener;

    static void cursorPosCallback(GLFWwindow* window, double xposIn, double yposIn);
    static void mouseButtonCallback(GLFWwindow* window, int buttonIn, int action, int mods);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

private:
    static bool m_firstMouse;
    static EventBus m_EventBus;


};


#endif //MOUSE_H
