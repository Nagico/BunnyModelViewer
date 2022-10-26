#ifndef MOUSE_H
#define MOUSE_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLFW/glfw3.h"

#include "Event.h"


class Mouse {
public:
    Mouse();

    void update();

    glm::vec2 position{};
    glm::vec2 scroll{};
    std::map<MouseButton, bool> state;

    void cursorPosCallback(double xposIn, double yposIn);
    void mouseButtonCallback(int buttonIn, int action, int mods);
    void scrollCallback(double xoffset, double yoffset);

private:

    bool m_firstMouse;
    EventBus m_EventBus;

};


#endif //MOUSE_H
