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
        eventbus.Subscribe<MouseButtonPressedEvent>(this, &Input::OnButtonPressedEvent);
        eventbus.Subscribe<MouseButtonReleasedEvent>(this, &Input::OnButtonReleasedEvent);
        eventbus.Subscribe<MouseMotionEvent>(this, &Input::OnMouseMovedEvent);
        eventbus.Subscribe<MouseWheelEvent>(this, &Input::OnScrolledEvent);
        
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

            // Initialize action duration if this is the first key pressed for this action
            if (!actionDurations.contains(keyBinding->actionType))
            {
                actionDurations[keyBinding->actionType] = 0.0f;
            }

            // Ensure the action exists
            if (!actions.contains(keyBinding->actionType))
            {
                actions[keyBinding->actionType] = std::make_unique<InputAction>();
            }

            const auto& action = actions[keyBinding->actionType];
            float2 value = CalculateActionValue(keyBinding->actionType);
            float duration = actionDurations[keyBinding->actionType];

            for (auto& callback : action->onStartedCallbacks)
            {
                callback(InputValue(value, duration));
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

            // Ensure the action exists
            if (!actions.contains(keyBinding->actionType))
            {
                actions[keyBinding->actionType] = std::make_unique<InputAction>();
            }

            const auto& action = actions[keyBinding->actionType];
            float2 value = CalculateActionValue(keyBinding->actionType);

            // Check if any other keys for this action are still pressed
            bool actionStillActive = false;
            for (auto actionKey : actionKeys[keyBinding->actionType])
            {
                if (actionKey != key && pressedKeys.contains(actionKey))
                {
                    actionStillActive = true;
                    break;
                }
            }

            // Get duration before potentially resetting it
            float duration = actionDurations.contains(keyBinding->actionType) ? actionDurations[keyBinding->actionType] : 0.0f;

            for (auto& callback : action->onEndedCallbacks)
            {
                callback(InputValue(value, duration));
            }

            // Reset duration only if no other keys for this action are pressed
            if (!actionStillActive)
            {
                actionDurations.erase(keyBinding->actionType);
            }
        }

        pressedKeys.erase(key);
    }

    // Mouse
    void Input::OnButtonPressedEvent(const MouseButtonPressedEvent event)
    {
        #if defined(BLACKBOX_DEBUG_INPUT_ALL) || defined(BLACKBOX_DEBUG_INPUT_MOUSE)
            LogInput->Info("Pressed Mouse button: {}", static_cast<int>(event.button));
        #endif

        const InputKey key = {event.button};
        if (!keybinds.contains(key))
        {
            return;
        }

        keyStates[key].value = {1.0f, 0.0f}; // Pressed state
        for (const auto& keyBinding : keybinds[key])
        {
            if (!contexts.contains(keyBinding->contextType))
            {
                continue;
            }

            // Initialize action duration if this is the first key pressed for this action
            if (!actionDurations.contains(keyBinding->actionType))
            {
                actionDurations[keyBinding->actionType] = 0.0f;
            }

            // Ensure the action exists
            if (!actions.contains(keyBinding->actionType))
            {
                actions[keyBinding->actionType] = std::make_unique<InputAction>();
            }

            const auto& action = actions[keyBinding->actionType];
            float2 value = CalculateActionValue(keyBinding->actionType);
            float duration = actionDurations[keyBinding->actionType];

            for (auto& callback : action->onStartedCallbacks)
            {
                callback(InputValue(value, duration));
            }
        }

        pressedKeys.insert(key);
    }

    void Input::OnButtonReleasedEvent(const MouseButtonReleasedEvent event)
    {
        #if defined(BLACKBOX_DEBUG_INPUT_ALL) || defined(BLACKBOX_DEBUG_INPUT_MOUSE)
            LogInput->Info("Released Mouse button: {}", static_cast<int>(event.button));
        #endif

        const InputKey key = {event.button};
        if (!keybinds.contains(key))
        {
            return;
        }

        keyStates[key].value = {0.0f, 0.0f}; // Unpressed state
        for (const auto& keyBinding : keybinds[key])
        {
            if (!contexts.contains(keyBinding->contextType))
            {
                continue;
            }

            // Ensure the action exists
            if (!actions.contains(keyBinding->actionType))
            {
                actions[keyBinding->actionType] = std::make_unique<InputAction>();
            }

            const auto& action = actions[keyBinding->actionType];
            float2 value = CalculateActionValue(keyBinding->actionType);

            // Check if any other keys for this action are still pressed
            bool actionStillActive = false;
            for (auto actionKey : actionKeys[keyBinding->actionType])
            {
                if (actionKey != key && pressedKeys.contains(actionKey))
                {
                    actionStillActive = true;
                    break;
                }
            }

            // Get duration before potentially resetting it
            float duration = actionDurations.contains(keyBinding->actionType) ? actionDurations[keyBinding->actionType] : 0.0f;

            for (auto& callback : action->onEndedCallbacks)
            {
                callback(InputValue(value, duration));
            }

            // Reset duration only if no other keys for this action are pressed
            if (!actionStillActive)
            {
                actionDurations.erase(keyBinding->actionType);
            }
        }

        pressedKeys.erase(key);
    }

    void Input::OnMouseMovedEvent(const MouseMotionEvent event)
    {
        #if defined(BLACKBOX_DEBUG_INPUT_ALL) || defined(BLACKBOX_DEBUG_INPUT_MOUSE)
            LogInput->Info("Mouse moved: direction({}, {}), position({}, {})",
                event.direction.x, event.direction.y, event.position.x, event.position.y);
        #endif

        mousePosition = event.position;

        // Process as continuous input with the direction
        ProcessContinuousInput({Mouse::Motion::XY}, event.direction);
    }

    void Input::OnScrolledEvent(const MouseWheelEvent event)
    {
        #if defined(BLACKBOX_DEBUG_INPUT_ALL) || defined(BLACKBOX_DEBUG_INPUT_MOUSE)
            LogInput->Info("Mouse scrolled: {}", event.direction);
        #endif

        // Process as continuous input with vertical scroll direction
        ProcessContinuousInput({Mouse::Wheel::Vertical}, {event.direction, 0.0f});
    }

    // Helper methods
    float2 Input::CalculateActionValue(const std::type_index actionType)
    {
        float2 value = {0.0f, 0.0f};

        // Check if this action has any keys bound
        if (!actionKeys.contains(actionType))
        {
            return value;
        }

        for (auto actionKey : actionKeys[actionType])
        {
            // Check if this key has bindings
            if (!keybinds.contains(actionKey))
            {
                continue;
            }

            // Find the binding for this specific key to get its modifiers
            for (const auto& actionKeyBinding : keybinds.at(actionKey))
            {
                if (actionKeyBinding->actionType == actionType)
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
        return value;
    }

    void Input::ProcessContinuousInput(const InputKey& key, float2 rawValue)
    {
        if (!keybinds.contains(key))
        {
            return;
        }

        keyStates[key].value = rawValue;

        for (const auto& keyBinding : keybinds[key])
        {
            if (!contexts.contains(keyBinding->contextType))
            {
                continue;
            }

            // Ensure the action exists
            if (!actions.contains(keyBinding->actionType))
            {
                actions[keyBinding->actionType] = std::make_unique<InputAction>();
            }

            auto& continuousState = continuousInputs[keyBinding->actionType];
            const auto& action = actions[keyBinding->actionType];

            // Calculate the full action value with all modifiers
            float2 value = CalculateActionValue(keyBinding->actionType);

            // If this is the first input, trigger onStarted
            if (!continuousState.isActive)
            {
                continuousState.isActive = true;
                continuousState.duration = 0.0f;
                for (auto& callback : action->onStartedCallbacks)
                {
                    callback(InputValue(value, 0.0f));
                }
            }

            // Reset the debounce timer
            continuousState.timeSinceLastInput = 0.0f;
            continuousState.lastValue = value;

            // Trigger onTriggered callbacks
            for (auto& callback : action->onTriggeredCallbacks)
            {
                callback(InputValue(value, continuousState.duration));
            }
        }
    }

    // General
    void Input::ProcessHeldInputs(const TickEvent event)
    {
        // Track which actions we've already processed this frame
        std::unordered_set<std::type_index> processedActions;

        for (auto& key : pressedKeys)
        {
            if (!keybinds.contains(key))
            {
                continue;
            }

            for (const auto& keyBinding : keybinds[key])
            {
                if (!contexts.contains(keyBinding->contextType))
                {
                    continue; // Key does not trigger if none of the corresponding context are active
                }

                // Process each action only once per frame
                if (!processedActions.contains(keyBinding->actionType))
                {
                    actionDurations[keyBinding->actionType] += event.deltaTime;

                    // Ensure the action exists
                    if (!actions.contains(keyBinding->actionType))
                    {
                        actions[keyBinding->actionType] = std::make_unique<InputAction>();
                    }

                    const auto& action = actions[keyBinding->actionType];
                    float2 value = CalculateActionValue(keyBinding->actionType);
                    float duration = actionDurations[keyBinding->actionType];

                    for (auto& callback : action->onTriggeredCallbacks)
                    {
                        callback(InputValue(value, duration));
                    }

                    processedActions.insert(keyBinding->actionType);
                }
            }
        }

        // Process continuous inputs for debounced onEnded
        std::vector<std::type_index> toRemove;
        for (auto& [actionType, state] : continuousInputs)
        {
            if (!state.isActive)
            {
                continue;
            }

            // Increment timers
            state.timeSinceLastInput += event.deltaTime;
            state.duration += event.deltaTime;

            // Check if we've exceeded the debounce duration
            if (state.timeSinceLastInput >= CONTINUOUS_INPUT_DEBOUNCE)
            {
                // Ensure the action exists
                if (!actions.contains(actionType))
                {
                    actions[actionType] = std::make_unique<InputAction>();
                }

                const auto& action = actions[actionType];
                for (auto& callback : action->onEndedCallbacks)
                {
                    callback(InputValue(state.lastValue, state.duration));
                }

                // Mark for cleanup
                toRemove.push_back(actionType);
            }
        }

        // Clean up inactive continuous inputs
        for (const auto& actionType : toRemove)
        {
            continuousInputs.erase(actionType);
        }
    }
}
