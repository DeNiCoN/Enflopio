#pragma once
#include <chrono>
#include <thread>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/chrono.h>

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

        double GetDesiredDelta() const { return std::chrono::duration<double>(m_desired_delta).count(); }
        std::uint64_t GetSimulationTick() const { return m_simulation_tick; }
        std::uint64_t GetFrameTick() const { return m_frame_tick; }
    private:
        static T& Instance()
        {
            static T instance;
            return instance;
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

        bool ShouldStop() const { return false; }
        bool ShouldSleep() const { return false; }
        void PreUpdate() {};
        void PreSimulate(double delta) {};
        void Simulate(double delta) {};
        void PostSimulate(double delta) {};
        void Render(double delay) {};
        void PostUpdate() {};
        void Terminate() {};


        using TimePoint = std::chrono::high_resolution_clock::time_point;
        using Duration = std::chrono::high_resolution_clock::duration;
        TimePoint m_last_update;
        Duration m_desired_delta = std::chrono::duration_cast<Duration>(std::chrono::duration<double>(1.0/60.0));
        Duration m_lag = std::chrono::high_resolution_clock::duration::zero();
        TimePoint m_current_update;
        std::uint64_t m_frame_tick = 0;
        std::uint64_t m_simulation_tick = 0;

        void Frame()
        {
            using namespace std::chrono;

            AsT()->UpdateClock();
            bool simulation_needed = m_lag > m_desired_delta;

            AsT()->PreUpdate();

            if (simulation_needed)
            {
                AsT()->PreSimulate(duration<double>(m_desired_delta).count());
            }

            while (m_lag > m_desired_delta)
            {
                m_lag -= m_desired_delta;
                AsT()->Simulate(duration<double>(m_desired_delta).count());

                m_simulation_tick++;
            }

            if (simulation_needed)
            {
                AsT()->PostSimulate(duration<double>(m_desired_delta).count());
            }

            AsT()->Render(duration<double>(m_lag).count());
            AsT()->PostUpdate();

            m_frame_tick++;

            if (AsT()->ShouldSleep())
            {
                std::this_thread::sleep_for(m_desired_delta - m_lag);
            }
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
