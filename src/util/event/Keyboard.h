//
// Created by co on 2022/10/26.
//

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <map>

#include "enum.h"
#include "Event.h"

class Keyboard {
public:
    static void init(GLFWwindow *window);
    static void checkInLoop();

    static std::map<KeyboardKey, bool> state;

private:
    static EventBus m_EventBus;

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};


#endif //KEYBOARD_H
