#include "Input.hpp"

#include <iostream>

#include "Engine.hpp"
#include "Events.hpp"
#include "EventBus.hpp"
#include "InputKeys.hpp"

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
        // eventbus.Subscribe<MouseButtonPressedEvent>(this, &Input::OnButtonPressedEvent);
        // eventbus.Subscribe<MouseButtonReleasedEvent>(this, &Input::OnButtonReleasedEvent);
        // eventbus.Subscribe<MouseMotionEvent>(this, &Input::OnMouseMovedEvent);
        // eventbus.Subscribe<MouseWheelEvent>(this, &Input::OnScrolledEvent);
        
        // Controller
        // eventbus.Subscribe<FaceButtonPressedEvent>(this, &Input::OnFaceButtonPressedEvent);
        // eventbus.Subscribe<FaceButtonReleasedEvent>(this, &Input::OnFaceButtonReleasedEvent);
        // eventbus.Subscribe<ShoulderPressedEvent>(this, &Input::OnShoulderPressedEvent);
        // eventbus.Subscribe<ShoulderReleasedEvent>(this, &Input::OnShoulderReleasedEvent);
        // eventbus.Subscribe<TriggerEvent>(this, &Input::OnTriggerEvent);
        // eventbus.Subscribe<DPadPressedEvent>(this, &Input::OnDPadPressedEvent);
        // eventbus.Subscribe<DPadReleasedEvent>(this, &Input::OnDPadReleasedEvent);
        // eventbus.Subscribe<SpecialPressedEvent>(this, &Input::OnSpecialPressedEvent);
        // eventbus.Subscribe<SpecialReleasedEvent>(this, &Input::OnSpecialReleasedEvent);
        // eventbus.Subscribe<StickMotionEvent>(this, &Input::OnStickMotionEvent);
        // eventbus.Subscribe<StickPressedEvent>(this, &Input::OnStickPressedEvent);
        // eventbus.Subscribe<StickReleasedEvent>(this, &Input::OnStickReleasedEvent);
        
        eventbus.Subscribe<TickEvent>(this, &Input::ProcessHeldInputs);
    }

    void Input::RemoveAllContexts() { contexts.clear(); }

    // Keybaord
    void Input::OnKeyPressedEvent(const KeyPressedEvent event)
    {
        #if defined(BLACKBOX_DEBUG_INPUT_ALL) || defined(BLACKBOX_DEBUG_INPUT_KEYBOARD)
            LogInput->Info("Pressed Keyboard key: {}", to_string(event.key));
        #endif

        const InputKey key = {event.key};
        if (!keybinds.contains(key))
        {
            return;
        }

        keyStates[key].value = {1.0f, 0.0f}; // Pressed state
        for (const auto& keyBinding : keybinds[key])
        {
            if (!contexts.contains(keyBinding->contextType))
            {
                continue; // Key does not trigger if none of the corresponding context are active
            }

            const auto& action = actions[keyBinding->actionType];
            float2 value = {0.0f, 0.0f};
            for (auto actionKey : actionKeys[keyBinding->actionType])
            {
                // Find the binding for this specific key to get its modifiers
                for (const auto& actionKeyBinding : keybinds[actionKey])
                {
                    if (actionKeyBinding->actionType == keyBinding->actionType)
                    {
                        float2 keyValue = keyStates[actionKey].value;
                        for (const auto& mod : actionKeyBinding->modifiers)
                        {
                            keyValue = mod->Execute(keyValue);
                        }
                        value += keyValue;
                        break;
                    }
                }
            }
            
            for (auto& callback : action->onStartedCallbacks)
            {
                callback(value);
            }
        }

        pressedKeys.insert(key);
    }
    
    void Input::OnKeyReleasedEvent(const KeyReleasedEvent event)
    {
        #if defined(BLACKBOX_DEBUG_INPUT_ALL) || defined(BLACKBOX_DEBUG_INPUT_KEYBOARD)
        LogInput->Info("Released Keyboard key: {}", to_string(event.key));
        #endif
        
        const InputKey key = {event.key};
        if (!keybinds.contains(key))
        {
            return;
        }

        keyStates[key].value = {0.0f, 0.0f}; // Unpressed state
        for (const auto& keyBinding : keybinds[key])
        {
            if (!contexts.contains(keyBinding->contextType))
            {
                continue; // Key does not trigger if none of the corresponding context are active
            }

            const auto& action = actions[keyBinding->actionType];
            float2 value = {0.0f, 0.0f};
            for (auto actionKey : actionKeys[keyBinding->actionType])
            {
                // Find the binding for this specific key to get its modifiers
                for (const auto& actionKeyBinding : keybinds[actionKey])
                {
                    if (actionKeyBinding->actionType == keyBinding->actionType)
                    {
                        float2 keyValue = keyStates[actionKey].value;
                        for (const auto& mod : actionKeyBinding->modifiers)
                        {
                            keyValue = mod->Execute(keyValue);
                        }
                        value += keyValue;
                        break;
                    }
                }
            }

            for (auto& callback : action->onEndedCallbacks)
            {
                callback(value);
            }
        }

        pressedKeys.erase(key);
    }

    // General
    void Input::ProcessHeldInputs(const TickEvent event)
    {
        for (auto& key : pressedKeys)
        {
            if (!keybinds.contains(key))
            {
                continue;
            }
            
            // keyStates[key].duration += event.deltaTime;
            for (const auto& keyBinding : keybinds[key])
            {
                if (!contexts.contains(keyBinding->contextType))
                {
                    continue; // Key does not trigger if none of the corresponding context are active
                }

                const auto& action = actions[keyBinding->actionType];
                float2 value = {0.0f, 0.0f};
                for (auto actionKey : actionKeys[keyBinding->actionType])
                {
                    // Find the binding for this specific key to get its modifiers
                    for (const auto& actionKeyBinding : keybinds[actionKey])
                    {
                        if (actionKeyBinding->actionType == keyBinding->actionType)
                        {
                            float2 keyValue = keyStates[actionKey].value;
                            for (const auto& mod : actionKeyBinding->modifiers)
                            {
                                keyValue = mod->Execute(keyValue);
                            }
                            value += keyValue;
                            break;
                        }
                    }
                }

                for (auto& callback : action->onTriggeredCallbacks)
                {
                    callback(value);
                }
            }
        }
    }
}
