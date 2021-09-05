#include "repl.hpp"
#include <iostream>
#include <mutex>
#include <spdlog/spdlog.h>

namespace Enflopio
{
    void Repl::StartInput()
    {
        m_thread = std::thread([this]
        {
            while(true)
            {
                const char* s = m_replxx.input("> ");
                {
                    std::scoped_lock lock(m_messages_mutex);
                    m_messages.push_back(s);
                }
            }
        });
    }

    void Repl::StopInput()
    {

    }

    void Repl::ProcessCommands()
    {
        spdlog::get("frame")->warn("presimulate3");
        std::scoped_lock lock(m_messages_mutex);
        spdlog::get("frame")->warn("presimulate4");
        while (!m_messages.empty())
        {
            spdlog::get("frame")->warn("presimulate2");
            const auto& message = m_messages.front();
            Write(message);
            m_messages.pop_front();
        }
    }

    void Repl::Write(std::string_view view)
    {
        m_replxx.write(view.data(), view.size());
    }
}
