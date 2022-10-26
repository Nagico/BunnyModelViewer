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
        struct ClickPressEvent {
            glm::vec2 position;
        };

        template<MouseButton T>
        struct ClickHoldEvent {
            glm::vec2 position;
        };

        template<MouseButton T>
        struct ClickReleaseEvent {
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
        template<KeyboardKey T>
        struct KeyPressEvent {
        };

        template<KeyboardKey T>
        struct KeyHoldEvent {
        };

        template<KeyboardKey T>
        struct KeyReleaseEvent {
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

    void publishMouseClickPressEvent(MouseButton button, glm::vec2 position) {
        switch (button) {
            case MouseButton::LEFT:
                m_EventBus->postpone(event::Mouse::ClickPressEvent<MouseButton::LEFT>{position});
                break;
            case MouseButton::RIGHT:
                m_EventBus->postpone(event::Mouse::ClickPressEvent<MouseButton::RIGHT>{position});
                break;
            case MouseButton::MIDDLE:
                m_EventBus->postpone(event::Mouse::ClickPressEvent<MouseButton::MIDDLE>{position});
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

    void publishMouseClickReleaseEvent(MouseButton button, glm::vec2 position) {
        switch (button) {
            case MouseButton::LEFT:
                m_EventBus->postpone(event::Mouse::ClickReleaseEvent<MouseButton::LEFT>{position});
                break;
            case MouseButton::RIGHT:
                m_EventBus->postpone(event::Mouse::ClickReleaseEvent<MouseButton::RIGHT>{position});
                break;
            case MouseButton::MIDDLE:
                m_EventBus->postpone(event::Mouse::ClickReleaseEvent<MouseButton::MIDDLE>{position});
                break;
            default:
                break;
        }
    }

    void publishKeyPressEvent(KeyboardKey button) {
        switch (button) {
            case KeyboardKey::A:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardKey::A>{});
                break;
            case KeyboardKey::D:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardKey::D>{});
                break;
            case KeyboardKey::E:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardKey::E>{});
                break;
            case KeyboardKey::F:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardKey::F>{});
                break;
            case KeyboardKey::L:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardKey::L>{});
                break;
            case KeyboardKey::P:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardKey::P>{});
                break;
            case KeyboardKey::Q:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardKey::Q>{});
                break;
            case KeyboardKey::S:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardKey::S>{});
                break;
            case KeyboardKey::W:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardKey::W>{});
                break;
            case KeyboardKey::RIGHT:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardKey::RIGHT>{});
                break;
            case KeyboardKey::LEFT:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardKey::LEFT>{});
                break;
            case KeyboardKey::DOWN:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardKey::DOWN>{});
                break;
            case KeyboardKey::UP:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardKey::UP>{});
                break;
            case KeyboardKey::LEFT_SHIFT:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardKey::LEFT_SHIFT>{});
                break;
            case KeyboardKey::LEFT_CONTROL:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardKey::LEFT_CONTROL>{});
                break;
            case KeyboardKey::LEFT_ALT:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardKey::LEFT_ALT>{});
                break;
            case KeyboardKey::RIGHT_SHIFT:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardKey::RIGHT_SHIFT>{});
                break;
            case KeyboardKey::RIGHT_CONTROL:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardKey::RIGHT_CONTROL>{});
                break;
            case KeyboardKey::RIGHT_ALT:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardKey::RIGHT_ALT>{});
                break;
            case KeyboardKey::ECS:
                m_EventBus->postpone(event::Keyboard::KeyPressEvent<KeyboardKey::ECS>{});
                break;
            default:
                break;
        }
    }

    void publishKeyHoldEvent(KeyboardKey button) {
        switch (button) {
            case KeyboardKey::A:
                m_EventBus->postpone(event::Keyboard::KeyHoldEvent<KeyboardKey::A>{});
                break;
            case KeyboardKey::D:
                m_EventBus->postpone(event::Keyboard::KeyHoldEvent<KeyboardKey::D>{});
                break;
            case KeyboardKey::E:
                m_EventBus->postpone(event::Keyboard::KeyHoldEvent<KeyboardKey::E>{});
                break;
            case KeyboardKey::F:
                m_EventBus->postpone(event::Keyboard::KeyHoldEvent<KeyboardKey::F>{});
                break;
            case KeyboardKey::L:
                m_EventBus->postpone(event::Keyboard::KeyHoldEvent<KeyboardKey::L>{});
                break;
            case KeyboardKey::P:
                m_EventBus->postpone(event::Keyboard::KeyHoldEvent<KeyboardKey::P>{});
                break;
            case KeyboardKey::Q:
                m_EventBus->postpone(event::Keyboard::KeyHoldEvent<KeyboardKey::Q>{});
                break;
            case KeyboardKey::S:
                m_EventBus->postpone(event::Keyboard::KeyHoldEvent<KeyboardKey::S>{});
                break;
            case KeyboardKey::W:
                m_EventBus->postpone(event::Keyboard::KeyHoldEvent<KeyboardKey::W>{});
                break;
            case KeyboardKey::RIGHT:
                m_EventBus->postpone(event::Keyboard::KeyHoldEvent<KeyboardKey::RIGHT>{});
                break;
            case KeyboardKey::LEFT:
                m_EventBus->postpone(event::Keyboard::KeyHoldEvent<KeyboardKey::LEFT>{});
                break;
            case KeyboardKey::DOWN:
                m_EventBus->postpone(event::Keyboard::KeyHoldEvent<KeyboardKey::DOWN>{});
                break;
            case KeyboardKey::UP:
                m_EventBus->postpone(event::Keyboard::KeyHoldEvent<KeyboardKey::UP>{});
                break;
            case KeyboardKey::LEFT_SHIFT:
                m_EventBus->postpone(event::Keyboard::KeyHoldEvent<KeyboardKey::LEFT_SHIFT>{});
                break;
            case KeyboardKey::LEFT_CONTROL:
                m_EventBus->postpone(event::Keyboard::KeyHoldEvent<KeyboardKey::LEFT_CONTROL>{});
                break;
            case KeyboardKey::LEFT_ALT:
                m_EventBus->postpone(event::Keyboard::KeyHoldEvent<KeyboardKey::LEFT_ALT>{});
                break;
            case KeyboardKey::RIGHT_SHIFT:
                m_EventBus->postpone(event::Keyboard::KeyHoldEvent<KeyboardKey::RIGHT_SHIFT>{});
                break;
            case KeyboardKey::RIGHT_CONTROL:
                m_EventBus->postpone(event::Keyboard::KeyHoldEvent<KeyboardKey::RIGHT_CONTROL>{});
                break;
            case KeyboardKey::RIGHT_ALT:
                m_EventBus->postpone(event::Keyboard::KeyHoldEvent<KeyboardKey::RIGHT_ALT>{});
                break;
            case KeyboardKey::ECS:
                m_EventBus->postpone(event::Keyboard::KeyHoldEvent<KeyboardKey::ECS>{});
                break;
            default:
                break;
        }
    }

    void publishKeyReleaseEvent(KeyboardKey button) {
        switch (button) {
            case KeyboardKey::A:
                m_EventBus->postpone(event::Keyboard::KeyReleaseEvent<KeyboardKey::A>{});
                break;
            case KeyboardKey::D:
                m_EventBus->postpone(event::Keyboard::KeyReleaseEvent<KeyboardKey::D>{});
                break;
            case KeyboardKey::E:
                m_EventBus->postpone(event::Keyboard::KeyReleaseEvent<KeyboardKey::E>{});
                break;
            case KeyboardKey::F:
                m_EventBus->postpone(event::Keyboard::KeyReleaseEvent<KeyboardKey::F>{});
                break;
            case KeyboardKey::L:
                m_EventBus->postpone(event::Keyboard::KeyReleaseEvent<KeyboardKey::L>{});
                break;
            case KeyboardKey::P:
                m_EventBus->postpone(event::Keyboard::KeyReleaseEvent<KeyboardKey::P>{});
                break;
            case KeyboardKey::Q:
                m_EventBus->postpone(event::Keyboard::KeyReleaseEvent<KeyboardKey::Q>{});
                break;
            case KeyboardKey::S:
                m_EventBus->postpone(event::Keyboard::KeyReleaseEvent<KeyboardKey::S>{});
                break;
            case KeyboardKey::W:
                m_EventBus->postpone(event::Keyboard::KeyReleaseEvent<KeyboardKey::W>{});
                break;
            case KeyboardKey::RIGHT:
                m_EventBus->postpone(event::Keyboard::KeyReleaseEvent<KeyboardKey::RIGHT>{});
                break;
            case KeyboardKey::LEFT:
                m_EventBus->postpone(event::Keyboard::KeyReleaseEvent<KeyboardKey::LEFT>{});
                break;
            case KeyboardKey::DOWN:
                m_EventBus->postpone(event::Keyboard::KeyReleaseEvent<KeyboardKey::DOWN>{});
                break;
            case KeyboardKey::UP:
                m_EventBus->postpone(event::Keyboard::KeyReleaseEvent<KeyboardKey::UP>{});
                break;
            case KeyboardKey::LEFT_SHIFT:
                m_EventBus->postpone(event::Keyboard::KeyReleaseEvent<KeyboardKey::LEFT_SHIFT>{});
                break;
            case KeyboardKey::LEFT_CONTROL:
                m_EventBus->postpone(event::Keyboard::KeyReleaseEvent<KeyboardKey::LEFT_CONTROL>{});
                break;
            case KeyboardKey::LEFT_ALT:
                m_EventBus->postpone(event::Keyboard::KeyReleaseEvent<KeyboardKey::LEFT_ALT>{});
                break;
            case KeyboardKey::RIGHT_SHIFT:
                m_EventBus->postpone(event::Keyboard::KeyReleaseEvent<KeyboardKey::RIGHT_SHIFT>{});
                break;
            case KeyboardKey::RIGHT_CONTROL:
                m_EventBus->postpone(event::Keyboard::KeyReleaseEvent<KeyboardKey::RIGHT_CONTROL>{});
                break;
            case KeyboardKey::RIGHT_ALT:
                m_EventBus->postpone(event::Keyboard::KeyReleaseEvent<KeyboardKey::RIGHT_ALT>{});
                break;
            case KeyboardKey::ECS:
                m_EventBus->postpone(event::Keyboard::KeyReleaseEvent<KeyboardKey::ECS>{});
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
