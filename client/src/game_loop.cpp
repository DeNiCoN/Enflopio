#include "game_loop.hpp"
#include <chrono>
#include <spdlog/spdlog.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace Enflopio
{
    //TODO do something with this
    GameLoop* instance;
    void main_loop()
    {
        instance->Loop();
    }

    void GameLoop::Loop()
    {
        m_current_update = std::chrono::high_resolution_clock::now();
        m_lag += std::chrono::duration<double>(m_current_update - m_last_update).count();
        m_last_update = m_current_update;

        while (m_lag > desired_fps)
        {
            m_lag -= desired_fps;
            Update(desired_fps);
        }
        Render(m_lag);
    }

    void GameLoop::Run()
    {
        instance = this;

        Init();

        m_last_update = std::chrono::high_resolution_clock::now();

#ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(main_loop, 0, true);
#else
        while(!ShouldStop())
        {
            Loop();
        }

        Terminate();
#endif
    }
}
