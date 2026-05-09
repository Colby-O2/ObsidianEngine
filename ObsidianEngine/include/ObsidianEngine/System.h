#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "Registry.h"

namespace ObsidianEngine
{
    class ISystem 
    {
    public:
        virtual ~ISystem() = default;
        virtual void update(Registry& registry, float dt) = 0;
    };
}

#endif