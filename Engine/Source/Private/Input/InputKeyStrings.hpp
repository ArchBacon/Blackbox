#pragma once

#include "InputKeys.hpp"

namespace blackbox
{
    const char* to_string(const Keyboard e)
    {
        switch (e)
        {
        case Keyboard::A: return "A";
        case Keyboard::B: return "B";
        case Keyboard::C: return "C";
        case Keyboard::D: return "D";
        case Keyboard::E: return "E";
        case Keyboard::F: return "F";
        case Keyboard::G: return "G";
        case Keyboard::H: return "H";
        case Keyboard::I: return "I";
        case Keyboard::J: return "J";
        case Keyboard::K: return "K";
        case Keyboard::L: return "L";
        case Keyboard::M: return "M";
        case Keyboard::N: return "N";
        case Keyboard::O: return "O";
        case Keyboard::P: return "P";
        case Keyboard::Q: return "Q";
        case Keyboard::R: return "R";
        case Keyboard::S: return "S";
        case Keyboard::T: return "T";
        case Keyboard::U: return "U";
        case Keyboard::V: return "V";
        case Keyboard::W: return "W";
        case Keyboard::X: return "X";
        case Keyboard::Y: return "Y";
        case Keyboard::Z: return "Z";
        case Keyboard::Num0: return "Num0";
        case Keyboard::Num1: return "Num1";
        case Keyboard::Num2: return "Num2";
        case Keyboard::Num3: return "Num3";
        case Keyboard::Num4: return "Num4";
        case Keyboard::Num5: return "Num5";
        case Keyboard::Num6: return "Num6";
        case Keyboard::Num7: return "Num7";
        case Keyboard::Num8: return "Num8";
        case Keyboard::Num9: return "Num9";
        case Keyboard::F1: return "F1";
        case Keyboard::F2: return "F2";
        case Keyboard::F3: return "F3";
        case Keyboard::F4: return "F4";
        case Keyboard::F5: return "F5";
        case Keyboard::F6: return "F6";
        case Keyboard::F7: return "F7";
        case Keyboard::F8: return "F8";
        case Keyboard::F9: return "F9";
        case Keyboard::F10: return "F10";
        case Keyboard::F11: return "F11";
        case Keyboard::F12: return "F12";
        case Keyboard::Left: return "Left";
        case Keyboard::Right: return "Right";
        case Keyboard::Up: return "Up";
        case Keyboard::Down: return "Down";
        case Keyboard::Space: return "Space";
        case Keyboard::Enter: return "Enter";
        case Keyboard::Tab: return "Tab";
        case Keyboard::Backspace: return "Backspace";
        case Keyboard::Delete: return "Delete";
        case Keyboard::Escape: return "Escape";
        case Keyboard::LeftShift: return "LeftShift";
        case Keyboard::RightShift: return "RightShift";
        case Keyboard::LeftCtrl: return "LeftCtrl";
        case Keyboard::RightCtrl: return "RightCtrl";
        case Keyboard::LeftAlt: return "LeftAlt";
        case Keyboard::RightAlt: return "RightAlt";
        case Keyboard::LeftSuper: return "LeftSuper";
        case Keyboard::RightSuper: return "RightSuper";
        case Keyboard::CapsLock: return "CapsLock";
        case Keyboard::NumLock: return "NumLock";
        case Keyboard::ScrollLock: return "ScrollLock";
        case Keyboard::Home: return "Home";
        case Keyboard::End: return "End";
        case Keyboard::PageUp: return "PageUp";
        case Keyboard::PageDown: return "PageDown";
        case Keyboard::Insert: return "Insert";
        case Keyboard::Semicolon: return "Semicolon";
        case Keyboard::Equals: return "Equals";
        case Keyboard::Comma: return "Comma";
        case Keyboard::Minus: return "Minus";
        case Keyboard::Period: return "Period";
        case Keyboard::Slash: return "Slash";
        case Keyboard::Grave: return "Grave";
        case Keyboard::LeftBracket: return "LeftBracket";
        case Keyboard::Backslash: return "Backslash";
        case Keyboard::RightBracket: return "RightBracket";
        case Keyboard::Apostrophe: return "Apostrophe";
        case Keyboard::PrintScreen: return "PrintScreen";
        case Keyboard::Pause: return "Pause";
        case Keyboard::Menu: return "Menu";
        case Keyboard::KP_Num0: return "KP_Num0";
        case Keyboard::KP_Num1: return "KP_Num1";
        case Keyboard::KP_Num2: return "KP_Num2";
        case Keyboard::KP_Num3: return "KP_Num3";
        case Keyboard::KP_Num4: return "KP_Num4";
        case Keyboard::KP_Num5: return "KP_Num5";
        case Keyboard::KP_Num6: return "KP_Num6";
        case Keyboard::KP_Num7: return "KP_Num7";
        case Keyboard::KP_Num8: return "KP_Num8";
        case Keyboard::KP_Num9: return "KP_Num9";
        case Keyboard::KP_Divide: return "KP_Slash";
        case Keyboard::KP_Multiply: return "KP_Multiply";
        case Keyboard::KP_Minus: return "KP_Minus";
        case Keyboard::KP_Plus: return "KP_Plus";
        case Keyboard::KP_Enter: return "KP_Enter";
        case Keyboard::KP_Period: return "KP_Period";
        case Keyboard::ContextMenu: return "ContextMenu";
        default: return "unknown";
        }
    }

    const char* to_string(const Mouse::Button e)
    {
        switch (e)
        {
        case Mouse::Button::Left: return "Left Mouse Button";
        case Mouse::Button::Right: return "Right Mouse Button";
        case Mouse::Button::Middle: return "Middle Mouse Button";
        case Mouse::Button::X1: return "X1";
        case Mouse::Button::X2: return "X2";
        default: return "unknown";
        }
    }

    const char* to_string(const Mouse::Motion e)
    {
        switch (e)
        {
        case Mouse::Motion::XY: return "Mouse XY";
        default: return "unknown";
        }
    }

    const char* to_string(const Mouse::Wheel e)
    {
        switch (e)
        {
        case Mouse::Wheel::Vertical: return "Scroll Vertical";
        default: return "unknown";
        }
    }

    const char* to_string(const Controller::FaceButton e)
    {
        switch (e)
        {
        case Controller::FaceButton::Down: return "FaceButton Down";
        case Controller::FaceButton::Right: return "FaceButton Right";
        case Controller::FaceButton::Left: return "FaceButton Left";
        case Controller::FaceButton::Up: return "FaceButton Up";
        default: return "unknown";
        }
    }

    const char* to_string(const Controller::Shoulder e)
    {
        switch (e)
        {
        case Controller::Shoulder::Left: return "Left Shoulder";
        case Controller::Shoulder::Right: return "Right Shoulder";
        default: return "unknown";
        }
    }

    const char* to_string(const Controller::Trigger e)
    {
        switch (e)
        {
        case Controller::Trigger::Left: return "Left Trigger";
        case Controller::Trigger::Right: return "Right Trigger";
        default: return "unknown";
        }
    }

    const char* to_string(const Controller::DPad e)
    {
        switch (e)
        {
        case Controller::DPad::Up: return "DPad Up";
        case Controller::DPad::Down: return "DPad Down";
        case Controller::DPad::Left: return "DPad Left";
        case Controller::DPad::Right: return "DPad Right";
        default: return "unknown";
        }
    }

    const char* to_string(const Controller::Special e)
    {
        switch (e)
        {
        case Controller::Special::Left: return "Share";
        case Controller::Special::Right: return "Options";
        default: return "unknown";
        }
    }

    const char* to_string(const Controller::Stick::Motion e)
    {
        switch (e)
        {
        case Controller::Stick::Motion::Left: return "Left Stick Motion";
        case Controller::Stick::Motion::Right: return "Right Stick Motion";
        default: return "unknown";
        }
    }

    const char* to_string(const Controller::Stick::Pressed e)
    {
        switch (e)
        {
        case Controller::Stick::Pressed::Left: return "Left Stick Button";
        case Controller::Stick::Pressed::Right: return "Right Stick Button";
        default: return "unknown";
        }
    }
}
