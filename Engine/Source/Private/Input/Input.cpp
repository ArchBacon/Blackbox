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
        // Force all joysticks to be treated as gamepads if possible
        // Without this, Joystick events are triggered, but not gamepad events
        auto* gamepads = SDL_GetGamepads(nullptr);
        if (gamepads) {
            int count = 0;
            while (gamepads[count] != 0) count++;
    
            for (int i = 0; i < count; i++) {
                SDL_Gamepad* gamepad = SDL_OpenGamepad(gamepads[i]);
                if (gamepad) {
                    LogEngine->Info("Opened gamepad: {}", SDL_GetGamepadName(gamepad));
                    // Keep it open - don't close it here
                }
            }
            SDL_free(gamepads);
        }
        
        // Keyboard
        eventbus.Subscribe<KeyPressedEvent>(this, &Input::OnKeyPressedEvent);
        eventbus.Subscribe<KeyReleasedEvent>(this, &Input::OnKeyReleasedEvent);

        // Mouse
        eventbus.Subscribe<MouseButtonPressedEvent>(this, &Input::OnButtonPressedEvent);
        eventbus.Subscribe<MouseButtonReleasedEvent>(this, &Input::OnButtonReleasedEvent);
        eventbus.Subscribe<MouseMotionEvent>(this, &Input::OnMouseMovedEvent);
        eventbus.Subscribe<MouseWheelEvent>(this, &Input::OnScrolledEvent);

        // Controller
        eventbus.Subscribe<FaceButtonPressedEvent>(this, &Input::OnFaceButtonPressedEvent);
        eventbus.Subscribe<FaceButtonReleasedEvent>(this, &Input::OnFaceButtonReleasedEvent);
        eventbus.Subscribe<ShoulderPressedEvent>(this, &Input::OnShoulderPressedEvent);
        eventbus.Subscribe<ShoulderReleasedEvent>(this, &Input::OnShoulderReleasedEvent);
        eventbus.Subscribe<TriggerEvent>(this, &Input::OnTriggerEvent);
        eventbus.Subscribe<DPadPressedEvent>(this, &Input::OnDPadPressedEvent);
        eventbus.Subscribe<DPadReleasedEvent>(this, &Input::OnDPadReleasedEvent);
        eventbus.Subscribe<SpecialPressedEvent>(this, &Input::OnSpecialPressedEvent);
        eventbus.Subscribe<SpecialReleasedEvent>(this, &Input::OnSpecialReleasedEvent);
        eventbus.Subscribe<StickMotionEvent>(this, &Input::OnStickMotionEvent);
        eventbus.Subscribe<StickPressedEvent>(this, &Input::OnStickPressedEvent);
        eventbus.Subscribe<StickReleasedEvent>(this, &Input::OnStickReleasedEvent);
        
        eventbus.Subscribe<TickEvent>(this, &Input::OnTickEvent);
    }

    void Input::RemoveAllContexts() { contexts.clear(); }

    // Keybaord
    void Input::OnKeyPressedEvent(const KeyPressedEvent event)
    {
        #if defined(BLACKBOX_DEBUG_INPUT_ALL) || defined(BLACKBOX_DEBUG_INPUT_KEYBOARD)
            LogInput->Info("Pressed Keyboard key: {}", to_string(event.key));
        #endif

        HandlePressedInput({1.0f, 0.0f}, {event.key});
    }
    void Input::OnKeyReleasedEvent(const KeyReleasedEvent event)
    {
        #if defined(BLACKBOX_DEBUG_INPUT_ALL) || defined(BLACKBOX_DEBUG_INPUT_KEYBOARD)
        LogInput->Info("Released Keyboard key: {}", to_string(event.key));
        #endif
        
        HandleReleaseInput({0.0f, 0.0f}, {event.key});
    }

    // Mouse
    void Input::OnButtonPressedEvent(const MouseButtonPressedEvent event)
    {
        #if defined(BLACKBOX_DEBUG_INPUT_ALL) || defined(BLACKBOX_DEBUG_INPUT_MOUSE)
            LogInput->Info("Pressed mouse button: {}", to_string(event.button));
        #endif

        HandlePressedInput({1.0f, 0.0f}, {event.button});
    }
    void Input::OnButtonReleasedEvent(const MouseButtonReleasedEvent event)
    {
        HandleReleaseInput({0.0f, 0.0f}, {event.button});
    }
    void Input::OnMouseMovedEvent(const MouseMotionEvent event)
    {
        #if defined(BLACKBOX_DEBUG_INPUT_ALL) || defined(BLACKBOX_DEBUG_INPUT_MOUSE)
            LogInput->Info("Mouse Moved: ({}, {}) to ({}, {})", event.direction.x, event.direction.y, event.position.x, event.position.y);
        #endif
        
        mousePosition = event.position;
        HandlePressedInput(event.direction, {Mouse::Motion::XY});
    }
    void Input::OnScrolledEvent(const MouseWheelEvent event)
    {
        #if defined(BLACKBOX_DEBUG_INPUT_ALL) || defined(BLACKBOX_DEBUG_INPUT_MOUSE)
            LogInput->Info("Scrolled: {}", event.direction);
        #endif

        HandlePressedInput({event.direction, 0.0f}, {Mouse::Wheel::Vertical});
    }

    // Controller
    void Input::OnFaceButtonPressedEvent(FaceButtonPressedEvent event)
    {
        #if defined(BLACKBOX_DEBUG_INPUT_ALL) || defined(BLACKBOX_DEBUG_INPUT_CONTROLLER)
            LogInput->Info("Pressed controller button: {}", to_string(event.button));
        #endif

        HandlePressedInput({1.0f, 0.0f}, {event.button});
    }
    void Input::OnFaceButtonReleasedEvent(FaceButtonReleasedEvent event)
    {
        HandleReleaseInput({1.0f, 0.0f}, {event.button});
    }
    void Input::OnShoulderPressedEvent(ShoulderPressedEvent event)
    {
        #if defined(BLACKBOX_DEBUG_INPUT_ALL) || defined(BLACKBOX_DEBUG_INPUT_CONTROLLER)
        LogInput->Info("Pressed controller button: {}", to_string(event.shoulder));
        #endif

        HandlePressedInput({1.0f, 0.0f}, {event.shoulder});
    }
    void Input::OnShoulderReleasedEvent(ShoulderReleasedEvent event)
    {
        HandleReleaseInput({0.0f, 0.0f}, {event.shoulder});
    }
    void Input::OnTriggerEvent(TriggerEvent event)
    {
        HandlePressedInput({event.value, 0.0f}, {event.trigger});
    }
    void Input::OnDPadPressedEvent(DPadPressedEvent event)
    {
        #if defined(BLACKBOX_DEBUG_INPUT_ALL) || defined(BLACKBOX_DEBUG_INPUT_CONTROLLER)
        LogInput->Info("Pressed controller button: {}", to_string(event.button));
        #endif

        HandlePressedInput({1.0f, 0.0f}, {event.button});
    }
    void Input::OnDPadReleasedEvent(DPadReleasedEvent event)
    {
        HandlePressedInput({0.0f, 0.0f}, {event.button});
    }
    void Input::OnSpecialPressedEvent(SpecialPressedEvent event)
    {
        #if defined(BLACKBOX_DEBUG_INPUT_ALL) || defined(BLACKBOX_DEBUG_INPUT_CONTROLLER)
        LogInput->Info("Pressed controller button: {}", to_string(event.button));
        #endif

        HandlePressedInput({1.0f, 0.0f}, {event.button});
    }
    void Input::OnSpecialReleasedEvent(SpecialReleasedEvent event)
    {
        HandlePressedInput({0.0f, 0.0f}, {event.button});
    }
    void Input::OnStickMotionEvent(StickMotionEvent event)
    {
        HandlePressedInput(event.value, {event.stick});
    }
    void Input::OnStickPressedEvent(StickPressedEvent event)
    {
        #if defined(BLACKBOX_DEBUG_INPUT_ALL) || defined(BLACKBOX_DEBUG_INPUT_CONTROLLER)
        LogInput->Info("Pressed controller button: {}", to_string(event.stick));
        #endif

        HandlePressedInput({1.0f, 0.0f}, {event.stick});
    }
    void Input::OnStickReleasedEvent(StickReleasedEvent event)
    {
        HandlePressedInput({0.0f, 0.0f}, {event.stick});
    }

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

    void Input::HandlePressedInput(float2 value, const InputKey key)
    {
        if (!keybinds.contains(key))
        {
            return;
        }
        
        const auto binds = keybinds[key];
        if (!contexts.contains(binds->contextType))
        {
            return; // Key does not trigger if the corresponding context isn't active
        }
        
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

        activeKeys.insert(key);
    }
    void Input::HandleReleaseInput(float2 value, const InputKey key)
    {
        if (!keybinds.contains(key))
        {
            return;
        }
        
        const auto binds = keybinds[key];
        if (!contexts.contains(binds->contextType))
        {
            return;
        }
        
        for (const auto& mod : binds->modifiers)
        {
            value = mod->Execute(value);
        }
        lastActionsValue[binds->actionType] = value;
        
        const auto& action = actions[binds->actionType];
        for (auto& callback : action->onEndedCallbacks)
        {
            callback(value);
        }
        
        activeKeys.erase(key);
    }
}
