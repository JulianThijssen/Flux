/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** Component.h
** Declares the base class for all components
**
** Author: Julian Thijssen
** -------------------------------------------------------------------------*/

#pragma once

namespace Flux {
    class Component {
    public:
        virtual ~Component() { }
    };
}
