#include "process_manager.hpp"

namespace Enflopio
{
    void ProcessManager::Update(double delta)
    {
        auto process_it = m_processes.begin();
        while (process_it != m_processes.end())
        {
            auto& process_ptr = *process_it;
            if (process_ptr->GetState() == Process::State::UNINITIALIZED)
            {
                process_ptr->Init();
                process_ptr->state_ = Process::State::RUNNING;
                process_ptr->Update(delta);
            }
            else if (process_ptr->GetState() == Process::State::RUNNING)
            {
                process_ptr->Update(delta);
            }

            if (process_ptr->GetState() == Process::State::ENDED)
            {
                if (process_ptr->GetNext())
                {
                    Add(process_ptr->TakeNext());
                }
                process_it = m_processes.erase(process_it);
            }
            else
            {
                ++process_it;
            }
        }
    }
}
