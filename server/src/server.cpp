#include "server.hpp"
#include <chrono>
#include <span>
#include "messages.hpp"
#include <glm/gtc/random.hpp>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using namespace std;

namespace Enflopio
{
    Server::Server(const Options& options)
        : m_tcp_listener(options.tcp_port, *this),
          m_websocket_listener(options.websocket_port, *this)
    {

    }

    void Server::SetupLogging()
    {
        auto frame = spdlog::stdout_color_mt("frame");
        auto network = spdlog::stdout_color_mt("network");

        frame->enable_backtrace(NUM_BACKTRACE_LOG_MESSAGES);
        frame->set_level(spdlog::level::warn);

        network->enable_backtrace(NUM_BACKTRACE_LOG_MESSAGES);
        network->set_level(spdlog::level::warn);

        std::set_terminate([]
        {
            spdlog::get("frame")->dump_backtrace();
            spdlog::get("network")->dump_backtrace();
            std::abort();
        });
    }

    void Server::Init()
    {
        SetupLogging();

        StartListening();

        //TODO Read config, get world size, etc

        for (int i = 0; i < 100; i++)
        {
            World::Circle n = {.position = glm::linearRand(glm::vec2(0, 0), m_world.GetSize())};
            m_world.AddCircle(n);
        }
    }

    void Server::Terminate()
    {
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

    void Server::PreSimulate(double delta)
    {
        ProcessConnections();
        ProcessMessages();
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
            message.circles = m_world.GetCircles();
            message.last_input_id = protocol.LastInputId();
            message.last_input_delta = protocol.InputDelta();
            for (const auto& [id, player] : message.players)
            {
                spdlog::info("{}: ({}, {})", id, player.position.x, player.position.y);
            }
            connection->Send(Serialize(message));
        }
    }

    void Server::Simulate(double delta)
    {
        for (auto& [connection, protocol] : m_connections)
        {
            protocol.UpdateMove(delta);
        }

        m_world.Update(delta);
    }

    void Server::PostSimulate(double delta)
    {
        if (GetSimulationTick() % 4 == 0)
        {
            SendSync();
        }
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
