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
        struct IMC_Driving final : InputMappingContext<IMC_Driving>
        {
            IMC_Driving() : InputMappingContext({
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
        EventBus& eventbus;
        
        std::unordered_set<std::type_index> contexts {};
        std::unordered_set<InputKey, InputKeyHash> activeKeys {};
        std::unordered_map<std::type_index, std::unique_ptr<InputAction>> actions {};
        std::unordered_map<std::type_index, float2> lastActionsValue {};
        std::unordered_map<InputKey, std::shared_ptr<KeyBinding>, InputKeyHash> keybinds {};
        float2 mousePosition {};
        
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
        void OnTickEvent(TickEvent event);
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
        keybinds.merge(context.keybinds);

        if (!contexts.contains(type))
        {
            LogInput->Warn("Context `{}` couldn't be added.", type.name());
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
}
