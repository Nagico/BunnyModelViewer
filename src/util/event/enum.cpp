#include "enum.h"

MouseButton toMouseButton(int button)
{
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            return MouseButton::LEFT;
        case GLFW_MOUSE_BUTTON_RIGHT:
            return MouseButton::RIGHT;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            return MouseButton::MIDDLE;
        default:
            return MouseButton::LAST;
    }
}

int toGLFWMouseButton(MouseButton button)
{
    switch (button) {
        case MouseButton::LEFT:
            return GLFW_MOUSE_BUTTON_LEFT;
        case MouseButton::RIGHT:
            return GLFW_MOUSE_BUTTON_RIGHT;
        case MouseButton::MIDDLE:
            return GLFW_MOUSE_BUTTON_MIDDLE;
        default:
            return GLFW_MOUSE_BUTTON_LAST;
    }
}

KeyboardKey toKeyboardKey(int key)
{
    switch (key) {
        case GLFW_KEY_A:
            return KeyboardKey::A;
        case GLFW_KEY_D:
            return KeyboardKey::D;
        case GLFW_KEY_E:
            return KeyboardKey::E;
        case GLFW_KEY_F:
            return KeyboardKey::F;
        case GLFW_KEY_L:
            return KeyboardKey::L;
        case GLFW_KEY_P:
            return KeyboardKey::P;
        case GLFW_KEY_Q:
            return KeyboardKey::Q;
        case GLFW_KEY_S:
            return KeyboardKey::S;
        case GLFW_KEY_W:
            return KeyboardKey::W;
        case GLFW_KEY_RIGHT:
            return KeyboardKey::RIGHT;
        case GLFW_KEY_LEFT:
            return KeyboardKey::LEFT;
        case GLFW_KEY_DOWN:
            return KeyboardKey::DOWN;
        case GLFW_KEY_UP:
            return KeyboardKey::UP;
        case GLFW_KEY_LEFT_SHIFT:
            return KeyboardKey::LEFT_SHIFT;
        case GLFW_KEY_LEFT_CONTROL:
            return KeyboardKey::LEFT_CONTROL;
        case GLFW_KEY_LEFT_ALT:
            return KeyboardKey::LEFT_ALT;
        case GLFW_KEY_RIGHT_SHIFT:
            return KeyboardKey::RIGHT_SHIFT;
        case GLFW_KEY_RIGHT_CONTROL:
            return KeyboardKey::RIGHT_CONTROL;
        case GLFW_KEY_RIGHT_ALT:
            return KeyboardKey::RIGHT_ALT;
        case GLFW_KEY_ESCAPE:
            return KeyboardKey::ECS;
        default:
            return KeyboardKey::LAST;
    }
}

int toGLFWKeyboardKey(KeyboardKey key)
{
    switch (key) {
        case KeyboardKey::A:
            return GLFW_KEY_A;
        case KeyboardKey::D:
            return GLFW_KEY_D;
        case KeyboardKey::E:
            return GLFW_KEY_E;
        case KeyboardKey::F:
            return GLFW_KEY_F;
        case KeyboardKey::L:
            return GLFW_KEY_L;
        case KeyboardKey::P:
            return GLFW_KEY_P;
        case KeyboardKey::Q:
            return GLFW_KEY_Q;
        case KeyboardKey::S:
            return GLFW_KEY_S;
        case KeyboardKey::W:
            return GLFW_KEY_W;
        case KeyboardKey::RIGHT:
            return GLFW_KEY_RIGHT;
        case KeyboardKey::LEFT:
            return GLFW_KEY_LEFT;
        case KeyboardKey::DOWN:
            return GLFW_KEY_DOWN;
        case KeyboardKey::UP:
            return GLFW_KEY_UP;
        case KeyboardKey::LEFT_SHIFT:
            return GLFW_KEY_LEFT_SHIFT;
        case KeyboardKey::LEFT_CONTROL:
            return GLFW_KEY_LEFT_CONTROL;
        case KeyboardKey::LEFT_ALT:
            return GLFW_KEY_LEFT_ALT;
        case KeyboardKey::RIGHT_SHIFT:
            return GLFW_KEY_RIGHT_SHIFT;
        case KeyboardKey::RIGHT_CONTROL:
            return GLFW_KEY_RIGHT_CONTROL;
        case KeyboardKey::RIGHT_ALT:
            return GLFW_KEY_RIGHT_ALT;
        case KeyboardKey::ECS:
            return GLFW_KEY_ESCAPE;
        default:
            return GLFW_KEY_LAST;
    }
}