#include "server.hpp"
#include <chrono>
#include <span>
#include "messages.hpp"

using namespace std;

namespace Enflopio
{
    Server::Server(const Options& options)
        : m_tcp_listener(options.tcp_port, *this),
          m_websocket_listener(options.websocket_port, *this)
    {

    }

    void Server::Run()
    {
        StartListening();
        InitClock();

        while (!ShouldClose())
        {
                UpdateLoop();
        }

        StopListening();
    }

    void Server::StartListening()
    {
        m_tcp_listener.StartListening();
        m_websocket_listener.StartListening();
    }

    void Server::StopListening()
    {
        m_websocket_listener.StopListening();
        m_tcp_listener.StopListening();
    }

    void Server::InitClock()
    {
        m_current_update = chrono::high_resolution_clock::now();
    }

    void Server::UpdateLoop()
    {
        UpdateClock();
        ProcessMessages();
        while(m_lag > m_frame)
        {
            m_lag -= m_frame;
            UpdateGame(std::chrono::duration<double>(m_frame).count());
        }
        std::this_thread::sleep_for(m_frame - m_lag);
    }

    void Server::ProcessMessages()
    {
        for (auto& [connection, protocol] : m_connections)
        {
            while(connection->HasNext())
            {
                auto message = ServerMessages::Deserialize(connection->ReadNext());
                message->Accept(protocol);
            }
        }
    }

    void Server::UpdateGame(double delta)
    {

    }

    double Server::UpdateClock()
    {
        m_last_update = m_current_update;
        m_current_update = chrono::high_resolution_clock::now();
        auto delta = m_current_update - m_last_update;
        m_lag += delta;
        return chrono::duration<double>(delta).count();
    }

    void Server::NewConnection(Connection::Ptr connection)
    {
        std::lock_guard lock(m_connections_mutex);
        ProtocolImpl protocol(*connection);
        m_connections.push_back(std::make_pair(std::move(connection), protocol));
    }
}
