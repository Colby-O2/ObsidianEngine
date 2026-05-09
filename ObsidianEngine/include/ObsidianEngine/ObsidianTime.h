#ifndef OBSIDIAN_TIME_H_
#define OBSIDIAN_TIME_H_

namespace ObsidianEngine
{
    class ObsidianTime
    {
    public:
        static void update(float currentTime);

        static float deltaTime();
        static float time();

    private:
        inline static float s_lastTime = 0.0f;
        inline static float s_deltaTime = 0.0f;
        inline static float s_time = 0.0f;
    };
}

#endif