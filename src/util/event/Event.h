#ifndef EVENT_H
#define EVENT_H
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "eventbus/EventBus.hpp"
#include "enum.h"

using EventBus = std::shared_ptr<dexode::EventBus>;
using Listener = std::shared_ptr<dexode::EventBus::Listener>;

namespace event {
    struct Mouse {

        template<MouseButton T>
        struct ClickEvent {
            glm::vec2 position;
        };

        template<MouseButton T>
        struct ClickHoldEvent {
            glm::vec2 position;
        };

        struct ScrollEvent {
            glm::vec2 offset;
            glm::vec2 scroll;
        };

        struct MoveEvent {
            glm::vec2 offset;
            glm::vec2 position;
        };
    };

    struct Keyboard {
        template<KeyboardButton T>
        struct KeyPressEvent {
        };
    };
}

class EventHandler {
public:
    ~EventHandler() = default;
    EventHandler(const EventHandler&) = delete;
    EventHandler& operator=(const EventHandler&) = delete;
    static EventHandler& get() {
        static EventHandler instance;
        return instance;
    }

    Listener getListener() {
        return std::make_shared<dexode::EventBus::Listener>(m_EventBus);
    }

    EventBus getEventBus() {
        return m_EventBus;
    }

    template <class Event, typename _ = void>
    void addListener(std::function<void(const Event&)>&& callback) {
        m_Listener->listen(callback);
    }

    template <class EventCallback, typename Event = dexode::eventbus::internal::first_argument<EventCallback>>
    void addListener(EventCallback&& callback)
    {
        m_Listener->listen(callback);
    }

    void publishMouseClickEvent(MouseButton button, glm::vec2 position) {
        switch (button) {
            case MouseButton::LEFT:
                m_EventBus->postpone(event::Mouse::ClickEvent<MouseButton::LEFT>{position});
                break;
            case MouseButton::RIGHT:
                m_EventBus->postpone(event::Mouse::ClickEvent<MouseButton::RIGHT>{position});
                break;
            case MouseButton::MIDDLE:
                m_EventBus->postpone(event::Mouse::ClickEvent<MouseButton::MIDDLE>{position});
                break;
            default:
                break;
        }
    }

    void publishMouseClickHoldEvent(MouseButton button, glm::vec2 position) {
        switch (button) {
            case MouseButton::LEFT:
                m_EventBus->postpone(event::Mouse::ClickHoldEvent<MouseButton::LEFT>{position});
                break;
            case MouseButton::RIGHT:
                m_EventBus->postpone(event::Mouse::ClickHoldEvent<MouseButton::RIGHT>{position});
                break;
            case MouseButton::MIDDLE:
                m_EventBus->postpone(event::Mouse::ClickHoldEvent<MouseButton::MIDDLE>{position});
                break;
            default:
                break;
        }
    }

    void publishKeyPressEvent(KeyboardButton button) {
        switch (button) {
//            case KeyboardButton::SPACE:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::SPACE>{});
//                break;
//            case KeyboardButton::APOSTROPHE:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::APOSTROPHE>{});
//                break;
//            case KeyboardButton::COMMA:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::COMMA>{});
//                break;
//            case KeyboardButton::MINUS:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::MINUS>{});
//                break;
//            case KeyboardButton::PERIOD:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::PERIOD>{});
//                break;
//            case KeyboardButton::SLASH:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::SLASH>{});
//                break;
//            case KeyboardButton::NUM_0:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::NUM_0>{});
//                break;
//            case KeyboardButton::NUM_1:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::NUM_1>{});
//                break;
//            case KeyboardButton::NUM_2:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::NUM_2>{});
//                break;
//            case KeyboardButton::NUM_3:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::NUM_3>{});
//                break;
//            case KeyboardButton::NUM_4:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::NUM_4>{});
//                break;
//            case KeyboardButton::NUM_5:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::NUM_5>{});
//                break;
//            case KeyboardButton::NUM_6:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::NUM_6>{});
//                break;
//            case KeyboardButton::NUM_7:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::NUM_7>{});
//                break;
//            case KeyboardButton::NUM_8:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::NUM_8>{});
//                break;
//            case KeyboardButton::NUM_9:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::NUM_9>{});
//                break;
//            case KeyboardButton::SEMICOLON:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::SEMICOLON>{});
//                break;
//            case KeyboardButton::EQUAL:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::EQUAL>{});
//                break;
            case KeyboardButton::A:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::A>{});
                break;
            case KeyboardButton::B:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::B>{});
                break;
            case KeyboardButton::C:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::C>{});
                break;
            case KeyboardButton::D:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::D>{});
                break;
            case KeyboardButton::E:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::E>{});
                break;
            case KeyboardButton::F:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::F>{});
                break;
            case KeyboardButton::G:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::G>{});
                break;
            case KeyboardButton::H:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::H>{});
                break;
            case KeyboardButton::I:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::I>{});
                break;
            case KeyboardButton::J:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::J>{});
                break;
            case KeyboardButton::K:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::K>{});
                break;
            case KeyboardButton::L:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::L>{});
                break;
            case KeyboardButton::M:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::M>{});
                break;
            case KeyboardButton::N:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::N>{});
                break;
            case KeyboardButton::O:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::O>{});
                break;
            case KeyboardButton::P:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::P>{});
                break;
            case KeyboardButton::Q:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::Q>{});
                break;
            case KeyboardButton::R:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::R>{});
                break;
            case KeyboardButton::S:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::S>{});
                break;
            case KeyboardButton::T:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::T>{});
                break;
            case KeyboardButton::U:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::U>{});
                break;
            case KeyboardButton::V:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::V>{});
                break;
            case KeyboardButton::W:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::W>{});
                break;
            case KeyboardButton::X:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::X>{});
                break;
            case KeyboardButton::Y:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::Y>{});
                break;
            case KeyboardButton::Z:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::Z>{});
                break;
//            case KeyboardButton::LEFT_BRACKET:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::LEFT_BRACKET>{});
//                break;
//            case KeyboardButton::BACKSLASH:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::BACKSLASH>{});
//                break;
//            case KeyboardButton::RIGHT_BRACKET:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::RIGHT_BRACKET>{});
//                break;
//            case KeyboardButton::GRAVE_ACCENT:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::GRAVE_ACCENT>{});
//                break;
//            case KeyboardButton::WORLD_1:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::WORLD_1>{});
//                break;
//            case KeyboardButton::WORLD_2:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::WORLD_2>{});
//                break;
//            case KeyboardButton::ESCAPE:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::ESCAPE>{});
//                break;
//            case KeyboardButton::ENTER:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::ENTER>{});
//                break;
//            case KeyboardButton::TAB:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::TAB>{});
//                break;
//            case KeyboardButton::BACKSPACE:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::BACKSPACE>{});
//                break;
//            case KeyboardButton::INSERT:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::INSERT>{});
//                break;
//            case KeyboardButton::DELETE:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::DELETE>{});
//                break;
            case KeyboardButton::RIGHT:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::RIGHT>{});
                break;
            case KeyboardButton::LEFT:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::LEFT>{});
                break;
            case KeyboardButton::DOWN:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::DOWN>{});
                break;
            case KeyboardButton::UP:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::UP>{});
                break;
//            case KeyboardButton::PAGE_UP:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::PAGE_UP>{});
//                break;
//            case KeyboardButton::PAGE_DOWN:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::PAGE_DOWN>{});
//                break;
//            case KeyboardButton::HOME:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::HOME>{});
//                break;
//            case KeyboardButton::END:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::END>{});
//                break;
//            case KeyboardButton::CAPS_LOCK:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::CAPS_LOCK>{});
//                break;
//            case KeyboardButton::SCROLL_LOCK:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::SCROLL_LOCK>{});
//                break;
//            case KeyboardButton::NUM_LOCK:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::NUM_LOCK>{});
//                break;
//            case KeyboardButton::PRINT_SCREEN:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::PRINT_SCREEN>{});
//                break;
//            case KeyboardButton::PAUSE:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::PAUSE>{});
//                break;
//            case KeyboardButton::F1:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::F1>{});
//                break;
//            case KeyboardButton::F2:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::F2>{});
//                break;
//            case KeyboardButton::F3:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::F3>{});
//                break;
//            case KeyboardButton::F4:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::F4>{});
//                break;
//            case KeyboardButton::F5:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::F5>{});
//                break;
//            case KeyboardButton::F6:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::F6>{});
//                break;
//            case KeyboardButton::F7:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::F7>{});
//                break;
//            case KeyboardButton::F8:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::F8>{});
//                break;
//            case KeyboardButton::F9:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::F9>{});
//                break;
//            case KeyboardButton::F10:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::F10>{});
//                break;
//            case KeyboardButton::F11:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::F11>{});
//                break;
//            case KeyboardButton::F12:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::F12>{});
//                break;
//            case KeyboardButton::F13:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::F13>{});
//                break;
//            case KeyboardButton::F14:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::F14>{});
//                break;
//            case KeyboardButton::F15:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::F15>{});
//                break;
//            case KeyboardButton::F16:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::F16>{});
//                break;
//            case KeyboardButton::F17:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::F17>{});
//                break;
//            case KeyboardButton::F18:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::F18>{});
//                break;
//            case KeyboardButton::F19:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::F19>{});
//                break;
//            case KeyboardButton::F20:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::F20>{});
//                break;
//            case KeyboardButton::F21:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::F21>{});
//                break;
//            case KeyboardButton::F22:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::F22>{});
//                break;
//            case KeyboardButton::F23:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::F23>{});
//                break;
//            case KeyboardButton::F24:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::F24>{});
//                break;
//            case KeyboardButton::F25:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::F25>{});
//                break;
//            case KeyboardButton::KP_0:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::KP_0>{});
//                break;
//            case KeyboardButton::KP_1:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::KP_1>{});
//                break;
//            case KeyboardButton::KP_2:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::KP_2>{});
//                break;
//            case KeyboardButton::KP_3:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::KP_3>{});
//                break;
//            case KeyboardButton::KP_4:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::KP_4>{});
//                break;
//            case KeyboardButton::KP_5:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::KP_5>{});
//                break;
//            case KeyboardButton::KP_6:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::KP_6>{});
//                break;
//            case KeyboardButton::KP_7:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::KP_7>{});
//                break;
//            case KeyboardButton::KP_8:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::KP_8>{});
//                break;
//            case KeyboardButton::KP_9:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::KP_9>{});
//                break;
//            case KeyboardButton::KP_DECIMAL:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::KP_DECIMAL>{});
//                break;
//            case KeyboardButton::KP_DIVIDE:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::KP_DIVIDE>{});
//                break;
//            case KeyboardButton::KP_MULTIPLY:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::KP_MULTIPLY>{});
//                break;
//            case KeyboardButton::KP_SUBTRACT:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::KP_SUBTRACT>{});
//                break;
//            case KeyboardButton::KP_ADD:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::KP_ADD>{});
//                break;
//            case KeyboardButton::KP_ENTER:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::KP_ENTER>{});
//                break;
//            case KeyboardButton::KP_EQUAL:
//                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::KP_EQUAL>{});
//                break;
            case KeyboardButton::LEFT_SHIFT:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::LEFT_SHIFT>{});
                break;
            case KeyboardButton::LEFT_CONTROL:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::LEFT_CONTROL>{});
                break;
            case KeyboardButton::LEFT_ALT:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::LEFT_ALT>{});
                break;
            case KeyboardButton::LEFT_SUPER:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::LEFT_SUPER>{});
                break;
            case KeyboardButton::RIGHT_SHIFT:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::RIGHT_SHIFT>{});
                break;
            case KeyboardButton::RIGHT_CONTROL:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::RIGHT_CONTROL>{});
                break;
            case KeyboardButton::RIGHT_ALT:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::RIGHT_ALT>{});
                break;
            case KeyboardButton::RIGHT_SUPER:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::RIGHT_SUPER>{});
                break;
            case KeyboardButton::MENU:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardButton::MENU>{});
                break;
            default:
                break;
        }
    }

private:
    EventBus m_EventBus;
    Listener m_Listener;

    EventHandler() {
        m_EventBus = std::make_shared<dexode::EventBus>();
        m_Listener = std::make_shared<dexode::EventBus::Listener>(m_EventBus);
    }
};

#endif //EVENT_H
