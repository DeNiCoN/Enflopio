#pragma once

#pragma once
#include <memory>
#include <list>
#include <vector>

namespace Enflopio
{
    class ProcessManager;

    class Process
    {
    public:
        friend class ProcessManager;
        enum class State
        {
            UNINITIALIZED,
            RUNNING,
            ENDED
        };

        virtual void Init() {}
        virtual void Update(double delta) = 0;


        void SetNext(std::unique_ptr<Process> n) { next_ = std::move(n); }
        const Process* GetNext() const { return next_.get(); }
        Process* GetNext() { return next_.get(); }
        std::unique_ptr<Process> TakeNext() { return std::move(next_); };
        void Stop() { state_ = State::ENDED; }
        void Run() { state_ = State::RUNNING; }
        State GetState() { return state_; }

        Process() = default;
        virtual ~Process() = default;
        Process(Process&&) = default;
    private:
        State state_ = State::UNINITIALIZED;
        std::unique_ptr<Process> next_;
    };

    using ProcessPtr = std::unique_ptr<Process>;

    class ProcessManager
    {
    public:
        void Add(ProcessPtr ptr) { m_processes.push_back(std::move(ptr)); }
        void Update(double delta);
    private:
        std::list<ProcessPtr> m_processes;
    };
}
