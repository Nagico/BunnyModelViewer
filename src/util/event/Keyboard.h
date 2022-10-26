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
    Keyboard();
    void update();
    void keyCallback(int key, int scancode, int action, int mods);

    std::map<KeyboardKey, bool> state;

private:
    EventBus m_EventBus;
};


#endif //KEYBOARD_H
