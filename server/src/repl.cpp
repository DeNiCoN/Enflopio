#include "repl.hpp"
#include <iostream>
#include <iomanip>
#include <mutex>
#include <spdlog/spdlog.h>
#include <sstream>
#include "server.hpp"
#include "utils/strings.hpp"

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
        std::scoped_lock lock(m_messages_mutex);
        while (!m_messages.empty())
        {
            auto message = m_messages.front();
            m_messages.pop_front();

            if (message.empty())
                continue;

            replxx::Replxx::HistoryScan hs( m_replxx.history_scan() );
            for ( int i( 0 ); hs.next(); ++ i ) {
                std::cout << std::setw(4) << i << ": " << hs.get().text() << "\n";
			}

            auto args = utils::split<std::string_view>(message, " ");

            if (args[0] == "log")
            {
                if (args.size() > 1 && args[1] == "level")
                {
                    if (args.size() > 3)
                    {
                        std::shared_ptr<spdlog::logger> logger;
                        if (args[2] == "default")
                            logger = spdlog::default_logger();
                        else
                            logger = spdlog::get(std::string(args[2]));

                        if (logger)
                        {
                            logger->set_level(
                                spdlog::level::from_str(std::string(args[3])));
                            m_replxx.history_add(message);
                            continue;
                        }
                    }
                }
            }
            Write("Unrecognized command\n");
        }
    }

    void Repl::Write(std::string_view view)
    {
        m_replxx.write(view.data(), view.size());
    }
}
