/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** Log.cpp
** Implements a class containing static functions for logging
** into some output stream.
**
** Author: Julian Thijssen
** -------------------------------------------------------------------------*/

#include "Log.h"

#include <iostream>

namespace Flux {
    void Log::info(const std::string message) {
        std::cout << "Info: " + message << std::endl;
    }

    void Log::debug(const std::string message) {
        std::cout << "Debug: " + message << std::endl;
    }

    void Log::error(const std::string message) {
        std::cout << "Error: " + message << std::endl;
    }
}
