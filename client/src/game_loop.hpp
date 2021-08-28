#pragma once
#include <chrono>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace Enflopio
{
    //crtp
    template<typename T>
    class GameLoop
    {
    public:

        static T& Instance()
        {
            static T instance;
            return instance;
        }

        void Run()
        {
            AsT()->Init();

            AsT()->InitClock();

#ifdef __EMSCRIPTEN__
            emscripten_set_main_loop(main_loop, 0, true);
#else
            while(!AsT()->ShouldStop())
            {
                Frame();
            }

            AsT()->Terminate();
#endif
        }

        void InitClock()
        {
            m_last_update = std::chrono::high_resolution_clock::now();
        }

        void UpdateClock()
        {
            m_current_update = std::chrono::high_resolution_clock::now();
            m_lag += m_current_update - m_last_update;
            m_last_update = m_current_update;
        }

        bool ShouldStop() { return false; }
        void PreUpdate() {};
        void PostUpdate() {};
        void Simulate(double delta) {};
        void RenderUpdate() {};
        void Render(double delay) {};
        void Terminate() {};

        double GetDesiredDelta() { return std::chrono::duration<double>(m_desired_delta).count(); }
    private:
        using TimePoint = std::chrono::high_resolution_clock::time_point;
        using Duration = std::chrono::high_resolution_clock::duration;
        TimePoint m_last_update;
        Duration m_desired_delta = std::chrono::duration_cast<Duration>(std::chrono::duration<double>(1.0/60.0));
        Duration m_lag;
        TimePoint m_current_update;

        void Frame()
        {
            AsT()->UpdateClock();
            bool render_update_needed = m_lag > m_desired_delta;

            AsT()->PreUpdate();

            while (m_lag > m_desired_delta)
            {
                m_lag -= m_desired_delta;
                AsT()->Simulate(std::chrono::duration<double>(m_desired_delta).count());
            }

            if (render_update_needed)
                AsT()->RenderUpdate();

            AsT()->Render(std::chrono::duration<double>(m_lag).count());
            AsT()->PostUpdate();
        }

        static void main_loop()
        {
            GameLoop::Instance().Frame();
        }

        T* AsT() {
            return static_cast<T*>(this);
        }
    };
}
