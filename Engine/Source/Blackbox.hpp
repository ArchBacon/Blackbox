#pragma once

#include "Logger.hpp"

/** --------- DEBUG INPUT --------- **/
// #define BLACKBOX_DEBUG_INPUT_ALL
// #define BLACKBOX_DEBUG_INPUT_KEYBOARD
// #define BLACKBOX_DEBUG_INPUT_MOUSE
#define BLACKBOX_DEBUG_INPUT_CONTROLLER

inline auto LogEngine = std::make_unique<blackbox::Logger>("LogEngine");
inline auto LogRenderer = std::make_unique<blackbox::Logger>("LogRenderer");
inline auto LogInput = std::make_unique<blackbox::Logger>("LogInput");
