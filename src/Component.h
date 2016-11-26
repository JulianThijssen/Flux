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
#ifndef COMPONENT_H
#define COMPONENT_H

//#include <nim/engine/Entity.hpp>

namespace Flux {
    class Component {
    public:
        virtual ~Component() { }
        //Entity owner;
    };
}

#endif /* COMPONENT_H */
