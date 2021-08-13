#pragma once
#include <chrono>

namespace Enflopio
{
    class GameLoop
    {
    public:
        void Run();
        virtual ~GameLoop() = default;
    protected:
        virtual void Init() = 0;
        virtual void PreUpdate() = 0;
        virtual void Update(double delta) = 0;
        virtual void Render(double delay) = 0;
        virtual bool ShouldStop() = 0;
        virtual void Terminate() = 0;
    private:
        void Loop();

        double desired_fps = 1.0/60.0;

        using TimePoint = std::chrono::high_resolution_clock::time_point;
        using Duration = std::chrono::high_resolution_clock::duration;
        TimePoint m_last_update;
        double m_lag;
        TimePoint m_current_update;

        friend void main_loop();
    };
}
