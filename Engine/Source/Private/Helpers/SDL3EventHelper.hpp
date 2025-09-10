#pragma once

#include "Events.hpp"
#include "SDL3InputHelper.hpp"

namespace blackbox
{
    std::string to_string(SDL_EventType eventType);
    std::string to_string(const SDL_Event& event);
    
    // Convert SDL events into Blackbox events and dispatch those
    class SDL3ToBlackBoxEvent
    {
    public:
        static void Broadcast(SDL_Event& e, EventBus& eventbus)
        {
            switch (static_cast<SDL_EventType>(e.type))
            {
            // Engine events //
            case SDL_EVENT_QUIT:
                eventbus.Broadcast(ShutdownEvent {});
                break;
            // Window events //
            case SDL_EVENT_WINDOW_MINIMIZED:
                eventbus.Broadcast(WindowMinimizedEvent {});
                break;
            case SDL_EVENT_WINDOW_RESTORED:
                eventbus.Broadcast(WindowRestoredEvent {});
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                eventbus.Broadcast(WindowResizedEvent {.windowSize = {e.window.data1, e.window.data2}});
                break;
            case SDL_EVENT_WINDOW_FOCUS_LOST:
                eventbus.Broadcast(WindowFocusLostEvent {});
                break;
            case SDL_EVENT_WINDOW_FOCUS_GAINED:
                eventbus.Broadcast(WindowFocusGainedEvent {});
                break;
            case SDL_EVENT_WINDOW_MOUSE_ENTER:
                eventbus.Broadcast(WindowMouseEnterEvent {});
                break;
            case SDL_EVENT_WINDOW_MOUSE_LEAVE:
                eventbus.Broadcast(WindowMouseLeaveEvent {});
                break;
            // Keyboard events //
            case SDL_EVENT_KEY_DOWN:
                e.key.repeat
                    ? eventbus.Broadcast(KeyRepeatEvent {.key = MapSDLKeyboard(e.key.scancode) })
                    : eventbus.Broadcast(KeyPressedEvent {.key = MapSDLKeyboard(e.key.scancode) });
                break;
            case SDL_EVENT_KEY_UP:
                eventbus.Broadcast(KeyReleasedEvent {.key = MapSDLKeyboard(e.key.scancode) });
                break;
            // Mouse events //
            case SDL_EVENT_MOUSE_MOTION:
                eventbus.Broadcast(MouseMotionEvent {.direction = {e.motion.xrel, e.motion.yrel}, .position = {e.motion.x, e.motion.y}  });
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                eventbus.Broadcast(MouseButtonPressedEvent {.button = MapSDLMouseButton(e.button.button)});
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                eventbus.Broadcast(MouseButtonReleasedEvent {.button = MapSDLMouseButton(e.button.button)});
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                // Is the mouse wheel scrolled? some mouse wheels can click to left and right
                // if mouse wheel y is not 0, it was scrolled 
                if (e.wheel.y != 0.0f)
                {
                    eventbus.Broadcast(MouseWheelEvent {.direction = e.wheel.y});
                }
                break;
            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                switch (e.gbutton.button)
                {
                    // Face buttons //
                    case SDL_GAMEPAD_BUTTON_NORTH:
                        eventbus.Broadcast(FaceButtonPressedEvent {.button = Controller::FaceButton::Up});
                        break;
                    case SDL_GAMEPAD_BUTTON_EAST:
                        eventbus.Broadcast(FaceButtonPressedEvent {.button = Controller::FaceButton::Right});
                        break;
                    case SDL_GAMEPAD_BUTTON_SOUTH:
                        eventbus.Broadcast(FaceButtonPressedEvent {.button = Controller::FaceButton::Down});
                        break;
                    case SDL_GAMEPAD_BUTTON_WEST:
                        eventbus.Broadcast(FaceButtonPressedEvent {.button = Controller::FaceButton::Left});
                        break;
                    // Shoulders //
                    case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:
                        eventbus.Broadcast(ShoulderPressedEvent {.shoulder = Controller::Shoulder::Left});
                        break;
                    case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER:
                        eventbus.Broadcast(ShoulderPressedEvent {.shoulder = Controller::Shoulder::Right});
                        break;
                    // DPad //
                    case SDL_GAMEPAD_BUTTON_DPAD_UP:
                        eventbus.Broadcast(DPadPressedEvent {.button = Controller::DPad::Up});
                        break;
                    case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
                        eventbus.Broadcast(DPadPressedEvent {.button = Controller::DPad::Right});
                        break;
                    case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
                        eventbus.Broadcast(DPadPressedEvent {.button = Controller::DPad::Down});
                        break;
                    case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
                        eventbus.Broadcast(DPadPressedEvent {.button = Controller::DPad::Left});
                    break;
                    // Specials //
                    case SDL_GAMEPAD_BUTTON_START:
                        eventbus.Broadcast(SpecialPressedEvent {.button = Controller::Special::Options});
                        break;
                    case SDL_GAMEPAD_BUTTON_BACK:
                        eventbus.Broadcast(SpecialPressedEvent {.button = Controller::Special::Share});
                        break;
                    // Stick buttons //
                    case SDL_GAMEPAD_BUTTON_LEFT_STICK:
                        eventbus.Broadcast(StickPressedEvent {.stick = Controller::Stick::Pressed::Left});
                        break;
                    case SDL_GAMEPAD_BUTTON_RIGHT_STICK:
                        eventbus.Broadcast(StickPressedEvent {.stick = Controller::Stick::Pressed::Right});
                        break;
                    default:
                        break;
                }
                break;
            case SDL_EVENT_GAMEPAD_BUTTON_UP:
                switch (e.gbutton.button)
                {
                    // Face buttons //
                    case SDL_GAMEPAD_BUTTON_NORTH:
                        eventbus.Broadcast(FaceButtonReleasedEvent {.button = Controller::FaceButton::Up});
                        break;
                    case SDL_GAMEPAD_BUTTON_EAST:
                        eventbus.Broadcast(FaceButtonReleasedEvent {.button = Controller::FaceButton::Right});
                        break;
                    case SDL_GAMEPAD_BUTTON_SOUTH:
                        eventbus.Broadcast(FaceButtonReleasedEvent {.button = Controller::FaceButton::Down});
                        break;
                    case SDL_GAMEPAD_BUTTON_WEST:
                        eventbus.Broadcast(FaceButtonReleasedEvent {.button = Controller::FaceButton::Left});
                        break;
                    // Shoulders //
                    case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:
                        eventbus.Broadcast(ShoulderReleasedEvent {.shoulder = Controller::Shoulder::Left});
                        break;
                    case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER:
                        eventbus.Broadcast(ShoulderReleasedEvent {.shoulder = Controller::Shoulder::Right});
                        break;
                    // DPad //
                    case SDL_GAMEPAD_BUTTON_DPAD_UP:
                        eventbus.Broadcast(DPadReleasedEvent {.button = Controller::DPad::Up});
                        break;
                    case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
                        eventbus.Broadcast(DPadReleasedEvent {.button = Controller::DPad::Right});
                        break;
                    case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
                        eventbus.Broadcast(DPadReleasedEvent {.button = Controller::DPad::Down});
                        break;
                    case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
                        eventbus.Broadcast(DPadReleasedEvent {.button = Controller::DPad::Left});
                    break;
                    // Specials //
                    case SDL_GAMEPAD_BUTTON_START:
                        eventbus.Broadcast(SpecialReleasedEvent {.button = Controller::Special::Options});
                        break;
                    case SDL_GAMEPAD_BUTTON_BACK:
                        eventbus.Broadcast(SpecialReleasedEvent {.button = Controller::Special::Share});
                        break;
                    // Stick buttons //
                    case SDL_GAMEPAD_BUTTON_LEFT_STICK:
                        eventbus.Broadcast(StickReleasedEvent {.stick = Controller::Stick::Pressed::Left});
                        break;
                    case SDL_GAMEPAD_BUTTON_RIGHT_STICK:
                        eventbus.Broadcast(StickReleasedEvent {.stick = Controller::Stick::Pressed::Right});
                        break;
                    default:
                        break;
                }
                break;
            case SDL_EVENT_GAMEPAD_AXIS_MOTION:
                float value = glm::clamp((static_cast<float>(e.gaxis.value) / 32767.f), -1.f, 1.0f);
                switch (e.gaxis.axis)
                {
                    // Triggers //
                    case SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
                        eventbus.Broadcast(TriggerEvent {.trigger = Controller::Trigger::Left, .value = value});
                        break;
                    case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
                        eventbus.Broadcast(TriggerEvent {.trigger = Controller::Trigger::Right, .value = value});
                        break;
                    // Sticks //
                    case SDL_GAMEPAD_AXIS_LEFTX:
                    case SDL_GAMEPAD_AXIS_LEFTY: // TODO: Combine X & Y values
                        eventbus.Broadcast(StickMotionEvent {.stick = Controller::Stick::Motion::Left, .value});
                        break;
                    case SDL_GAMEPAD_AXIS_RIGHTX:
                    case SDL_GAMEPAD_AXIS_RIGHTY: // TODO: Combine X & Y values
                    eventbus.Broadcast(StickMotionEvent {.stick = Controller::Stick::Motion::Right, .value});
                        break;
                    default:
                        break;
                }
                break;
            // Ignored events //
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            case SDL_EVENT_WINDOW_SHOWN:
            case SDL_EVENT_CLIPBOARD_UPDATE:
            case SDL_EVENT_WINDOW_EXPOSED:
            case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
            case SDL_EVENT_WINDOW_MOVED:
                break;
            default:
                LogInput->Warn("Event {} not mapped.", to_string(e));
                break;
            }
        }
    };

    std::string to_string(SDL_EventType eventType)
    {
        switch (eventType)
        {
            // Application events
            case SDL_EVENT_QUIT: return "SDL_EVENT_QUIT";
            case SDL_EVENT_TERMINATING: return "SDL_EVENT_TERMINATING";
            case SDL_EVENT_LOW_MEMORY: return "SDL_EVENT_LOW_MEMORY";
            case SDL_EVENT_WILL_ENTER_BACKGROUND: return "SDL_EVENT_WILL_ENTER_BACKGROUND";
            case SDL_EVENT_DID_ENTER_BACKGROUND: return "SDL_EVENT_DID_ENTER_BACKGROUND";
            case SDL_EVENT_WILL_ENTER_FOREGROUND: return "SDL_EVENT_WILL_ENTER_FOREGROUND";
            case SDL_EVENT_DID_ENTER_FOREGROUND: return "SDL_EVENT_DID_ENTER_FOREGROUND";
            case SDL_EVENT_LOCALE_CHANGED: return "SDL_EVENT_LOCALE_CHANGED";
            case SDL_EVENT_SYSTEM_THEME_CHANGED: return "SDL_EVENT_SYSTEM_THEME_CHANGED";

            // Display events
            case SDL_EVENT_DISPLAY_ORIENTATION: return "SDL_EVENT_DISPLAY_ORIENTATION";
            case SDL_EVENT_DISPLAY_ADDED: return "SDL_EVENT_DISPLAY_ADDED";
            case SDL_EVENT_DISPLAY_REMOVED: return "SDL_EVENT_DISPLAY_REMOVED";
            case SDL_EVENT_DISPLAY_MOVED: return "SDL_EVENT_DISPLAY_MOVED";
            case SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED: return "SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED";
            case SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED: return "SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED";
            case SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED: return "SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED";

            // Window events
            case SDL_EVENT_WINDOW_SHOWN: return "SDL_EVENT_WINDOW_SHOWN";
            case SDL_EVENT_WINDOW_HIDDEN: return "SDL_EVENT_WINDOW_HIDDEN";
            case SDL_EVENT_WINDOW_EXPOSED: return "SDL_EVENT_WINDOW_EXPOSED";
            case SDL_EVENT_WINDOW_MOVED: return "SDL_EVENT_WINDOW_MOVED";
            case SDL_EVENT_WINDOW_RESIZED: return "SDL_EVENT_WINDOW_RESIZED";
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: return "SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED";
            case SDL_EVENT_WINDOW_MINIMIZED: return "SDL_EVENT_WINDOW_MINIMIZED";
            case SDL_EVENT_WINDOW_MAXIMIZED: return "SDL_EVENT_WINDOW_MAXIMIZED";
            case SDL_EVENT_WINDOW_RESTORED: return "SDL_EVENT_WINDOW_RESTORED";
            case SDL_EVENT_WINDOW_MOUSE_ENTER: return "SDL_EVENT_WINDOW_MOUSE_ENTER";
            case SDL_EVENT_WINDOW_MOUSE_LEAVE: return "SDL_EVENT_WINDOW_MOUSE_LEAVE";
            case SDL_EVENT_WINDOW_FOCUS_GAINED: return "SDL_EVENT_WINDOW_FOCUS_GAINED";
            case SDL_EVENT_WINDOW_FOCUS_LOST: return "SDL_EVENT_WINDOW_FOCUS_LOST";
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED: return "SDL_EVENT_WINDOW_CLOSE_REQUESTED";
            case SDL_EVENT_WINDOW_HIT_TEST: return "SDL_EVENT_WINDOW_HIT_TEST";
            case SDL_EVENT_WINDOW_ICCPROF_CHANGED: return "SDL_EVENT_WINDOW_ICCPROF_CHANGED";
            case SDL_EVENT_WINDOW_DISPLAY_CHANGED: return "SDL_EVENT_WINDOW_DISPLAY_CHANGED";
            case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED: return "SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED";
            case SDL_EVENT_WINDOW_OCCLUDED: return "SDL_EVENT_WINDOW_OCCLUDED";
            case SDL_EVENT_WINDOW_ENTER_FULLSCREEN: return "SDL_EVENT_WINDOW_ENTER_FULLSCREEN";
            case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN: return "SDL_EVENT_WINDOW_LEAVE_FULLSCREEN";
            case SDL_EVENT_WINDOW_DESTROYED: return "SDL_EVENT_WINDOW_DESTROYED";
            case SDL_EVENT_WINDOW_HDR_STATE_CHANGED: return "SDL_EVENT_WINDOW_HDR_STATE_CHANGED";

            // Keyboard events
            case SDL_EVENT_KEY_DOWN: return "SDL_EVENT_KEY_DOWN";
            case SDL_EVENT_KEY_UP: return "SDL_EVENT_KEY_UP";
            case SDL_EVENT_TEXT_EDITING: return "SDL_EVENT_TEXT_EDITING";
            case SDL_EVENT_TEXT_INPUT: return "SDL_EVENT_TEXT_INPUT";
            case SDL_EVENT_KEYMAP_CHANGED: return "SDL_EVENT_KEYMAP_CHANGED";
            case SDL_EVENT_KEYBOARD_ADDED: return "SDL_EVENT_KEYBOARD_ADDED";
            case SDL_EVENT_KEYBOARD_REMOVED: return "SDL_EVENT_KEYBOARD_REMOVED";

            // Mouse events
            case SDL_EVENT_MOUSE_MOTION: return "SDL_EVENT_MOUSE_MOTION";
            case SDL_EVENT_MOUSE_BUTTON_DOWN: return "SDL_EVENT_MOUSE_BUTTON_DOWN";
            case SDL_EVENT_MOUSE_BUTTON_UP: return "SDL_EVENT_MOUSE_BUTTON_UP";
            case SDL_EVENT_MOUSE_WHEEL: return "SDL_EVENT_MOUSE_WHEEL";
            case SDL_EVENT_MOUSE_ADDED: return "SDL_EVENT_MOUSE_ADDED";
            case SDL_EVENT_MOUSE_REMOVED: return "SDL_EVENT_MOUSE_REMOVED";

            // Joystick events
            case SDL_EVENT_JOYSTICK_AXIS_MOTION: return "SDL_EVENT_JOYSTICK_AXIS_MOTION";
            case SDL_EVENT_JOYSTICK_BALL_MOTION: return "SDL_EVENT_JOYSTICK_BALL_MOTION";
            case SDL_EVENT_JOYSTICK_HAT_MOTION: return "SDL_EVENT_JOYSTICK_HAT_MOTION";
            case SDL_EVENT_JOYSTICK_BUTTON_DOWN: return "SDL_EVENT_JOYSTICK_BUTTON_DOWN";
            case SDL_EVENT_JOYSTICK_BUTTON_UP: return "SDL_EVENT_JOYSTICK_BUTTON_UP";
            case SDL_EVENT_JOYSTICK_ADDED: return "SDL_EVENT_JOYSTICK_ADDED";
            case SDL_EVENT_JOYSTICK_REMOVED: return "SDL_EVENT_JOYSTICK_REMOVED";
            case SDL_EVENT_JOYSTICK_BATTERY_UPDATED: return "SDL_EVENT_JOYSTICK_BATTERY_UPDATED";
            case SDL_EVENT_JOYSTICK_UPDATE_COMPLETE: return "SDL_EVENT_JOYSTICK_UPDATE_COMPLETE";

            // Gamepad events (modern controller interface)
            case SDL_EVENT_GAMEPAD_AXIS_MOTION: return "SDL_EVENT_GAMEPAD_AXIS_MOTION";
            case SDL_EVENT_GAMEPAD_BUTTON_DOWN: return "SDL_EVENT_GAMEPAD_BUTTON_DOWN";
            case SDL_EVENT_GAMEPAD_BUTTON_UP: return "SDL_EVENT_GAMEPAD_BUTTON_UP";
            case SDL_EVENT_GAMEPAD_ADDED: return "SDL_EVENT_GAMEPAD_ADDED";
            case SDL_EVENT_GAMEPAD_REMOVED: return "SDL_EVENT_GAMEPAD_REMOVED";
            case SDL_EVENT_GAMEPAD_REMAPPED: return "SDL_EVENT_GAMEPAD_REMAPPED";
            case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN: return "SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN";
            case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION: return "SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION";
            case SDL_EVENT_GAMEPAD_TOUCHPAD_UP: return "SDL_EVENT_GAMEPAD_TOUCHPAD_UP";
            case SDL_EVENT_GAMEPAD_SENSOR_UPDATE: return "SDL_EVENT_GAMEPAD_SENSOR_UPDATE";
            case SDL_EVENT_GAMEPAD_UPDATE_COMPLETE: return "SDL_EVENT_GAMEPAD_UPDATE_COMPLETE";
            case SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED: return "SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED";

            // Touch events
            case SDL_EVENT_FINGER_DOWN: return "SDL_EVENT_FINGER_DOWN";
            case SDL_EVENT_FINGER_UP: return "SDL_EVENT_FINGER_UP";
            case SDL_EVENT_FINGER_MOTION: return "SDL_EVENT_FINGER_MOTION";

            // Clipboard events
            case SDL_EVENT_CLIPBOARD_UPDATE: return "SDL_EVENT_CLIPBOARD_UPDATE";

            // Drag and drop events
            case SDL_EVENT_DROP_FILE: return "SDL_EVENT_DROP_FILE";
            case SDL_EVENT_DROP_TEXT: return "SDL_EVENT_DROP_TEXT";
            case SDL_EVENT_DROP_BEGIN: return "SDL_EVENT_DROP_BEGIN";
            case SDL_EVENT_DROP_COMPLETE: return "SDL_EVENT_DROP_COMPLETE";
            case SDL_EVENT_DROP_POSITION: return "SDL_EVENT_DROP_POSITION";

            // Audio events
            case SDL_EVENT_AUDIO_DEVICE_ADDED: return "SDL_EVENT_AUDIO_DEVICE_ADDED";
            case SDL_EVENT_AUDIO_DEVICE_REMOVED: return "SDL_EVENT_AUDIO_DEVICE_REMOVED";
            case SDL_EVENT_AUDIO_DEVICE_FORMAT_CHANGED: return "SDL_EVENT_AUDIO_DEVICE_FORMAT_CHANGED";

            // Sensor events
            case SDL_EVENT_SENSOR_UPDATE: return "SDL_EVENT_SENSOR_UPDATE";

            // Pen events (stylus/tablet)
            case SDL_EVENT_PEN_DOWN: return "SDL_EVENT_PEN_DOWN";
            case SDL_EVENT_PEN_UP: return "SDL_EVENT_PEN_UP";
            case SDL_EVENT_PEN_MOTION: return "SDL_EVENT_PEN_MOTION";
            case SDL_EVENT_PEN_BUTTON_DOWN: return "SDL_EVENT_PEN_BUTTON_DOWN";
            case SDL_EVENT_PEN_BUTTON_UP: return "SDL_EVENT_PEN_BUTTON_UP";

            // Camera events
            case SDL_EVENT_CAMERA_DEVICE_ADDED: return "SDL_EVENT_CAMERA_DEVICE_ADDED";
            case SDL_EVENT_CAMERA_DEVICE_REMOVED: return "SDL_EVENT_CAMERA_DEVICE_REMOVED";
            case SDL_EVENT_CAMERA_DEVICE_APPROVED: return "SDL_EVENT_CAMERA_DEVICE_APPROVED";
            case SDL_EVENT_CAMERA_DEVICE_DENIED: return "SDL_EVENT_CAMERA_DEVICE_DENIED";

            // Render events
            case SDL_EVENT_RENDER_TARGETS_RESET: return "SDL_EVENT_RENDER_TARGETS_RESET";
            case SDL_EVENT_RENDER_DEVICE_RESET: return "SDL_EVENT_RENDER_DEVICE_RESET";

            // User events (custom events)
            case SDL_EVENT_USER: return "SDL_EVENT_USER";

            default: 
                return "UNKNOWN_EVENT(" + std::to_string(eventType) + ")";
        }
    }

    std::string to_string(const SDL_Event& event)
    {
        return to_string(static_cast<SDL_EventType>(event.type));
    }
}
