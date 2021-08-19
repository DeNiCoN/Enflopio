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
            m_tick++;
            Frame();

            if (m_tick % 4 == 0)
            {
                SendSync();
            }
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

    void Server::Frame()
    {
        //Update clock
        UpdateClock();
        double delta = std::chrono::duration<double>(m_frame).count();
        if (m_lag > m_frame)
        {
            ProcessConnections();
            ProcessMessages();
        }

        while(m_lag > m_frame)
        {
            m_lag -= m_frame;
            UpdateGame(delta);

            for (auto& [connection, protocol] : m_connections)
            {
                protocol.UpdateMove(delta);
            }
        }
        std::this_thread::sleep_for(m_frame - m_lag);
    }

    void Server::ProcessMessages()
    {
        for (auto& [connection, protocol] : m_connections)
        {
            while(connection->HasNext())
            {
                spdlog::debug("New message");
                auto message = ServerMessages::Deserialize(connection->ReadNext());
                message->Accept(protocol);
            }
        }
    }

    void Server::SendSync()
    {
        for (auto& [connection, protocol] : m_connections)
        {
            ClientMessages::Sync message;
            message.players = m_world.GetPlayers();
            message.last_input_id = protocol.LastInputId();
            message.last_input_delta = protocol.LastInputDelta();
            protocol.ResetMove();
            for (const auto& [id, player] : message.players)
            {
                spdlog::info("{}: ({}, {})", id, player.position.x, player.position.y);
            }
            connection->Send(Serialize(message));
        }
    }

    void Server::UpdateGame(double delta)
    {
        m_world.Update(delta);
    }

    double Server::UpdateClock()
    {
        m_last_update = m_current_update;
        m_current_update = chrono::high_resolution_clock::now();
        auto delta = m_current_update - m_last_update;
        m_lag += delta;
        return chrono::duration<double>(delta).count();
    }

    void Server::InitClock()
    {
        m_current_update = chrono::high_resolution_clock::now();
        m_lag = std::chrono::seconds(0);
    }

    void Server::PendingConnect(Connection::Ptr ptr)
    {
        std::scoped_lock lock(m_connections_mutex);
        m_pending_connect.push_back(std::move(ptr));
    }

    void Server::PendingDisconnect(Connection::Ptr ptr)
    {
        std::scoped_lock lock(m_connections_mutex);
        m_pending_disconnect.push_back(std::move(ptr));
    }

    void Server::NewConnect(Connection::Ptr connection)
    {
        spdlog::debug("New connection");
        ProtocolImpl protocol(*connection, m_world);
        m_connections.insert(std::make_pair(std::move(connection), std::move(protocol)));
    }

    void Server::Disconnect(Connection::Ptr connection)
    {
        spdlog::debug("Disconnect");
        auto it = m_connections.find(connection);
        it->second.Disconnect();
        m_connections.erase(connection);
    }

    void Server::ProcessConnections()
    {
        std::scoped_lock lock(m_connections_mutex);
        for (auto& ptr : m_pending_connect)
        {
            NewConnect(std::move(ptr));
        }
        m_pending_connect.clear();

        for (auto& ptr : m_pending_disconnect)
        {
            Disconnect(std::move(ptr));
        }
        m_pending_disconnect.clear();

    }
}
