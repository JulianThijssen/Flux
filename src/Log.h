/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** Log.h
** Declares a class containing static functions for logging
** into some output stream.
**
** Author: Julian Thijssen
** -------------------------------------------------------------------------*/

#pragma once
#ifndef LOG_H
#define LOG_H

#include <string>

class Log {
public:
    static void info(const std::string message);
    static void debug(const std::string message);
    static void error(const std::string message);
};

#endif /* LOG_H */
