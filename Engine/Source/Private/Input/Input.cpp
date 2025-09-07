#include "Input.hpp"

#include "Engine.hpp"
#include "Events.hpp"
#include "EventBus.hpp"
#include "InputKeyStrings.hpp"

namespace blackbox
{
    Input::Input(EventBus& eventbus)
        : eventbus(eventbus)
    {
        // Keyboard
        eventbus.Subscribe<KeyPressedEvent>(this, &Input::OnKeyPressedEvent);
        eventbus.Subscribe<KeyReleasedEvent>(this, &Input::OnKeyReleasedEvent);

        // Mouse
        eventbus.Subscribe<MouseButtonPressedEvent>(this, &Input::OnButtonPressedEvent);
        eventbus.Subscribe<MouseButtonReleasedEvent>(this, &Input::OnButtonReleasedEvent);
        eventbus.Subscribe<MouseMotionEvent>(this, &Input::OnMouseMovedEvent);
        eventbus.Subscribe<MouseWheelEvent>(this, &Input::OnScrolledEvent);
        
        eventbus.Subscribe<TickEvent>(this, &Input::OnTickEvent);
    }

    void Input::RemoveAllContexts() { contexts.clear(); }

    // Keybaord
    void Input::OnKeyPressedEvent(const KeyPressedEvent event)
    {
        #if defined(BLACKBOX_DEBUG_INPUT_ALL) || defined(BLACKBOX_DEBUG_INPUT_KEYBOARD)
            LogInput->Info("Pressed Keyboard key: {}", to_string(event.key));
        #endif
        
        if (!keybinds.contains({event.key}))
        {
            return;
        }
        
        const auto binds = keybinds[{event.key}];
        if (!contexts.contains(binds->contextType))
        {
            return; // Key does not trigger if the corresponding context isn't active
        }
        
        float2 value {1.0f, 0.0f};
        for (const auto& mod : binds->modifiers)
        {
            value = mod->Execute(value);
        }
        lastActionsValue[binds->actionType] = value;

        const auto& action = actions[binds->actionType];
        action->duration = 0.0f;
        for (auto& callback : action->onStartedCallbacks)
        {
            callback(value);
        }

        activeKeys.insert({event.key});
    }
    void Input::OnKeyReleasedEvent(const KeyReleasedEvent event)
    {
        if (!keybinds.contains({event.key}))
        {
            return;
        }
        
        const auto binds = keybinds[{event.key}];
        if (!contexts.contains(binds->contextType))
        {
            return;
        }
        
        float2 value {0.0f, 0.0f};
        for (const auto& mod : binds->modifiers)
        {
            value = mod->Execute(value);
        }
        lastActionsValue[binds->actionType] = value;
        
        const auto& action = actions[binds->actionType];
        for (auto& callback : action->onEndedCallbacks)
        {
            callback({value});
        }
        
        activeKeys.erase({event.key});
    }

    // Mouse
    void Input::OnButtonPressedEvent(const MouseButtonPressedEvent event)
    {
        #if defined(BLACKBOX_DEBUG_INPUT_ALL) || defined(BLACKBOX_DEBUG_INPUT_MOUSE)
            LogInput->Info("Pressed mouse button: {}", to_string(event.button));
        #endif
        
        if (!keybinds.contains({event.button}))
        {
            return;
        }
        
        const auto binds = keybinds[{event.button}];
        if (!contexts.contains(binds->contextType))
        {
            return; // Key does not trigger if the corresponding context isn't active
        }
        
        float2 value {1.0f, 0.0f};
        for (const auto& mod : binds->modifiers)
        {
            value = mod->Execute(value);
        }
        lastActionsValue[binds->actionType] = value;

        const auto& action = actions[binds->actionType];
        action->duration = 0.0f;
        for (auto& callback : action->onStartedCallbacks)
        {
            callback(value);
        }

        activeKeys.insert({event.button});
    }
    void Input::OnButtonReleasedEvent(const MouseButtonReleasedEvent event)
    {
        if (!keybinds.contains({event.button}))
        {
            return;
        }
        
        const auto binds = keybinds[{event.button}];
        if (!contexts.contains(binds->contextType))
        {
            return;
        }
        
        float2 value {0.0f, 0.0f};
        for (const auto& mod : binds->modifiers)
        {
            value = mod->Execute(value);
        }
        lastActionsValue[binds->actionType] = value;
        
        const auto& action = actions[binds->actionType];
        for (auto& callback : action->onEndedCallbacks)
        {
            callback({value});
        }
        
        activeKeys.erase({event.button});
    }
    void Input::OnMouseMovedEvent(const MouseMotionEvent event)
    {
        #if defined(BLACKBOX_DEBUG_INPUT_ALL) || defined(BLACKBOX_DEBUG_INPUT_MOUSE)
            LogInput->Info("Mouse Moved: ({}, {}) to ({}, {})", event.direction.x, event.direction.y, event.position.x, event.position.y);
        #endif

        mousePosition = event.position;
        
        if (!keybinds.contains({Mouse::Motion::XY}))
        {
            return;
        }
        
        const auto binds = keybinds[{Mouse::Motion::XY}];
        if (!contexts.contains(binds->contextType))
        {
            return; // Key does not trigger if the corresponding context isn't active
        }
        
        float2 value = event.direction;
        for (const auto& mod : binds->modifiers)
        {
            value = mod->Execute(value);
        }
        lastActionsValue[binds->actionType] = value;

        const auto& action = actions[binds->actionType];
        action->duration = 0.0f;
        for (auto& callback : action->onStartedCallbacks)
        {
            callback(value);
        }

        activeKeys.insert({Mouse::Motion::XY});
    }
    void Input::OnScrolledEvent(const MouseWheelEvent event)
    {
        #if defined(BLACKBOX_DEBUG_INPUT_ALL) || defined(BLACKBOX_DEBUG_INPUT_MOUSE)
            LogInput->Info("Scrolled: {}", event.direction);
        #endif
        
        if (!keybinds.contains({Mouse::Wheel::Vertical}))
        {
            return;
        }
        
        const auto binds = keybinds[{Mouse::Wheel::Vertical}];
        if (!contexts.contains(binds->contextType))
        {
            return; // Key does not trigger if the corresponding context isn't active
        }
        
        float2 value {event.direction, 0.0f};
        for (const auto& mod : binds->modifiers)
        {
            value = mod->Execute(value);
        }
        lastActionsValue[binds->actionType] = value;

        const auto& action = actions[binds->actionType];
        action->duration = 0.0f;
        for (auto& callback : action->onStartedCallbacks)
        {
            callback(value);
        }

        activeKeys.insert({Mouse::Wheel::Vertical});
    }

    // Controller
    void Input::OnFaceButtonPressedEvent(FaceButtonPressedEvent event)
    {
        #if defined(BLACKBOX_DEBUG_INPUT_ALL) || defined(BLACKBOX_DEBUG_INPUT_CONTROLLER)
        LogInput->Info("Pressed controller button: {}", to_string(event.button));
        #endif
        
        if (!keybinds.contains({event.button}))
        {
            return;
        }
        
        const auto binds = keybinds[{event.button}];
        if (!contexts.contains(binds->contextType))
        {
            return; // Key does not trigger if the corresponding context isn't active
        }
        
        float2 value {1.0f, 0.0f};
        for (const auto& mod : binds->modifiers)
        {
            value = mod->Execute(value);
        }
        lastActionsValue[binds->actionType] = value;

        const auto& action = actions[binds->actionType];
        action->duration = 0.0f;
        for (auto& callback : action->onStartedCallbacks)
        {
            callback(value);
        }

        activeKeys.insert({event.button});
    }
    void Input::OnFaceButtonReleasedEvent(FaceButtonReleasedEvent event) {}
    void Input::OnShoulderPressedEvent(ShoulderPressedEvent event) {}
    void Input::OnShoulderReleasedEvent(ShoulderReleasedEvent event) {}
    void Input::OnTriggerEvent(TriggerEvent event) {}
    void Input::OnDPadPressedEvent(DPadPressedEvent event) {}
    void Input::OnDPadReleasedEvent(DPadReleasedEvent event) {}
    void Input::OnSpecialPressedEvent(SpecialPressedEvent event) {}
    void Input::OnSpecialReleasedEvent(SpecialReleasedEvent event) {}
    void Input::OnStickMotionEvent(StickMotionEvent event) {}
    void Input::OnStickPressedEvent(StickPressedEvent event) {}
    void Input::OnStickReleasedEvent(StickReleasedEvent event) {}

    // General
    void Input::OnTickEvent(const TickEvent event)
    {
        for (auto& key : activeKeys)
        {
            if (!keybinds.contains(key))
            {
                continue;
            }
            
            const auto binds = keybinds[key];
            if (!contexts.contains(binds->contextType))
            {
                continue;
            }
        
            const auto& action = actions[binds->actionType];
            action->duration += event.deltaTime;
            for (auto& callback : action->onTriggeredCallbacks)
            {
                callback({lastActionsValue[binds->actionType]});
            }
        }
    }
}
