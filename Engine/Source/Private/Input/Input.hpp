#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>

#include "Blackbox.hpp"
#include "Events.hpp"
#include "InputAction.hpp"
#include "InputKeys.hpp"
#include "InputMapping.hpp"
#include "InputMappingContext.hpp"
#include "KeyBinding.hpp"

namespace blackbox
{
    class EventBus;
    struct KeyReleasedEvent;
    struct KeyPressedEvent;
    struct TickEvent;

    /**
    Example for defining an Input mapping context
     
        struct SteeringAction {};
        struct ThrottleAction {};
        struct DrivingInputContext final : InputMappingContext<DrivingInputContext>
        {
            DrivingInputContext() : InputMappingContext({
                InputMapping<SteeringAction> {
                    {Keyboard::A, Swizzle{}, Negate{}},
                    {Keyboard::D, Swizzle{}},
                    {Controller::Stick::Motion::Left, Deadzone{0.2f}},
                },
                InputMapping<ThrottleAction> {
                    {Keyboard::W},
                    {Keyboard::S, Negate{}},
                },
            }) {}
        };
    */
    class Input
    {
        struct KeyValue
        {
            float2 value {0.0f, 0.0f};
        };

        struct ContinuousInputState
        {
            bool isActive {false};
            float timeSinceLastInput {0.0f};
            float2 lastValue {0.0f, 0.0f};
            float duration {0.0f}; // total duration of this continuous input session
        };
        
        EventBus& eventbus;
        
        std::unordered_set<std::type_index> contexts {};
        std::unordered_map<std::type_index, std::unique_ptr<InputAction>> actions {};
        std::unordered_map<InputKey, std::vector<std::shared_ptr<KeyBinding>>, InputKeyHash> keybinds {}; // all bindings on contexts with the input key
        std::unordered_map<InputKey, KeyValue, InputKeyHash> keyStates {}; // last known values of keys
        std::unordered_set<InputKey, InputKeyHash> pressedKeys {}; // keys currently pressed
        std::unordered_map<std::type_index, std::vector<InputKey>> actionKeys {}; // keys that belong to an action
        std::unordered_map<std::type_index, float> actionDurations {}; // how long each action has been active
        std::unordered_map<std::type_index, ContinuousInputState> continuousInputs {}; // track continuous inputs for debounced onEnded
        float2 mousePosition {};

        static constexpr float CONTINUOUS_INPUT_DEBOUNCE = 0.1f; // Time to wait before triggering onEnded (100ms)
    
    public:
        Input(EventBus& eventbus);
        ~Input() = default;

        Input(const Input& other) = delete;
        Input& operator=(const Input&) = delete;
        Input(Input&& other) = delete;
        Input& operator=(Input&& other) = delete;

        template <InputMappingContextType T>
        void AddContext();
        
        template <InputMappingContextType T>
        void RemoveContext();

        // Disable all active contexts
        void RemoveAllContexts();

        template <typename T>
        [[nodiscard]] InputAction& GetAction();

        template <typename T>
        [[nodiscard]] InputValue GetActionValue();

        [[nodiscard]] float2 GetMousePosition() { return mousePosition; }

    private:
        // Keyboard
        void OnKeyPressedEvent(KeyPressedEvent event);
        void OnKeyReleasedEvent(KeyReleasedEvent event);
        
        // Mouse
        void OnButtonPressedEvent(MouseButtonPressedEvent event);
        void OnButtonReleasedEvent(MouseButtonReleasedEvent event);
        void OnMouseMovedEvent(MouseMotionEvent event);
        void OnScrolledEvent(MouseWheelEvent event);
        
        // Controller
        void OnFaceButtonPressedEvent(FaceButtonPressedEvent event);
        void OnFaceButtonReleasedEvent(FaceButtonReleasedEvent event);
        void OnShoulderPressedEvent(ShoulderPressedEvent event);
        void OnShoulderReleasedEvent(ShoulderReleasedEvent event);
        void OnTriggerEvent(TriggerEvent event);
        void OnDPadPressedEvent(DPadPressedEvent event);
        void OnDPadReleasedEvent(DPadReleasedEvent event);
        void OnSpecialPressedEvent(SpecialPressedEvent event);
        void OnSpecialReleasedEvent(SpecialReleasedEvent event);
        void OnStickMotionEvent(StickMotionEvent event);
        void OnStickPressedEvent(StickPressedEvent event);
        void OnStickReleasedEvent(StickReleasedEvent event);

        // General
        void ProcessHeldInputs(TickEvent event);

        // Helper methods
        float2 CalculateActionValue(std::type_index actionType);
        void ProcessContinuousInput(const InputKey& key, float2 rawValue);
    };

    template <InputMappingContextType T>
    void Input::AddContext()
    {
        const auto type = std::type_index(typeid(T));
        if (contexts.contains(type))
        {
            LogInput->Warn("Context `{}` is already active.", type.name());
            return;
        }
        
        InputMappingContext context = T();
        contexts.insert(type);
        
        // add intput keys in the arrays that belong to the actions
        for (auto keybind : context.keybinds)
        {
            for (auto keyBinding : keybind.second)
            {
                auto it = std::ranges::find(actionKeys[keyBinding->actionType], keybind.first);

                if (it == actionKeys[keyBinding->actionType].end())
                {
                    actionKeys[keyBinding->actionType].push_back(keybind.first);
                }
            } 
        }

        // Manually merge the vectors for each key to support multiple contexts binding the same key
        for (auto& [key, bindings] : context.keybinds)
        {
            keybinds[key].insert(keybinds[key].end(), bindings.begin(), bindings.end());
        }
    }

    template <InputMappingContextType T>
    void Input::RemoveContext()
    {
        const auto type = std::type_index(typeid(T));
        if (!contexts.contains(type))
        {
            LogInput->Warn("Context `{}` is not active.", type.name());
            return;
        }
        
        contexts.erase(type);
    }

    template <typename T>
    InputAction& Input::GetAction()
    {
        const auto type = std::type_index(typeid(T));
        if (!actions.contains(type))
        {
            actions[type] = std::make_unique<InputAction>();
        }

        return *actions[type];
    }

    template <typename T>
    InputValue Input::GetActionValue()
    {
        const auto type = std::type_index(typeid(T));
        float2 value = CalculateActionValue(type);
        float duration = actionDurations.contains(type) ? actionDurations[type] : 0.0f;
        return InputValue(value, duration);
    }
}
