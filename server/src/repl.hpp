#pragma once
#include <replxx.hxx>
#include <thread>
#include <shared_mutex>
#include <string_view>
#include <deque>

namespace Enflopio
{
    class Server;

    class Repl
    {
    public:
        Repl(Server& server)
            : m_server(server)
        {
        }

        void StartInput();
        void ProcessCommands();
        void StopInput();

        void Write(std::string_view);
    private:
        Server& m_server;
        std::thread m_thread;
        replxx::Replxx m_replxx;
        std::shared_mutex m_messages_mutex;
        std::deque<std::string> m_messages;
    };
}
