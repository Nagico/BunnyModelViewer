#ifndef ENUM_H
#define ENUM_H

#include "GLFW/glfw3.h"

enum class MouseButton {
    LEFT,
    RIGHT,
    MIDDLE,
    LAST
};

MouseButton toMouseButton(int button);

int toGLFWMouseButton(MouseButton button);

enum class KeyboardKey {
    A,
    D,
    E,
    F,
    L,
    P,
    Q,
    S,
    W,
    RIGHT,
    LEFT,
    DOWN,
    UP,
    LEFT_SHIFT,
    LEFT_CONTROL,
    LEFT_ALT,
    RIGHT_SHIFT,
    RIGHT_CONTROL,
    RIGHT_ALT,
    ECS,
    LAST
};

KeyboardKey toKeyboardKey(int key);

int toGLFWKeyboardKey(KeyboardKey key);

#endif //ENUM_H
