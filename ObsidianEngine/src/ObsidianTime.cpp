#include "ObsidianTime.h"

namespace ObsidianEngine
{
    void ObsidianTime::update(float currentTime)
    {
        s_deltaTime = currentTime - s_lastTime;
        s_lastTime = currentTime;
        s_time = currentTime;
    }

    float ObsidianTime::deltaTime()
    {
        return s_deltaTime;
    }

    float ObsidianTime::time()
    {
        return s_time;
    }
}