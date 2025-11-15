#pragma once

#include "InputValue.hpp"

namespace blackbox
{
    class InputAction
    {
        friend class Input;
        
        std::vector<std::function<void(InputValue)>> onStartedCallbacks {};
        std::vector<std::function<void(InputValue)>> onEndedCallbacks {};
        std::vector<std::function<void(InputValue)>> onTriggeredCallbacks {};
        
    public:
        template <typename Class>
        void OnStarted(Class* instance, void (Class::*method)(InputValue));
        void OnStarted(std::function<void(InputValue)> callback);
        
        template <typename Class>
        void OnEnded(Class* instance, void (Class::*method)(InputValue));
        void OnEnded(std::function<void(InputValue)> callback);
        
        template <typename Class>
        void OnTriggered(Class* instance, void (Class::*method)(InputValue));
        void OnTriggered(std::function<void(InputValue)> callback);
    };

    template <typename T>
    concept InputActionType = std::is_base_of_v<InputAction, T> && !std::is_same_v<InputAction, T>;

    template <typename Class>
    void InputAction::OnStarted(Class* instance, void(Class::* method)(InputValue))
    {
        auto callback = [instance, method](InputValue value)
        {
            (instance->*method)(value);
        };
        onStartedCallbacks.push_back(callback);
    }
    
    inline void InputAction::OnStarted(std::function<void(InputValue)> callback)
    {
        onStartedCallbacks.push_back(std::move(callback));
    }

    template <typename Class>
    void InputAction::OnEnded(Class* instance, void(Class::* method)(InputValue))
    {
        auto callback = [instance, method](InputValue value)
        {
            (instance->*method)(value);
        };
        onEndedCallbacks.push_back(callback);
    }
    
    inline void InputAction::OnEnded(std::function<void(InputValue)> callback)
    {
        onEndedCallbacks.push_back(std::move(callback));
    }

    template <typename Class>
    void InputAction::OnTriggered(Class* instance, void(Class::* method)(InputValue))
    {
        auto callback = [instance, method](InputValue value)
        {
            (instance->*method)(value);
        };
        onTriggeredCallbacks.push_back(callback);
    }
    
    inline void InputAction::OnTriggered(std::function<void(InputValue)> callback)
    {
        onTriggeredCallbacks.push_back(std::move(callback));
    }
}
