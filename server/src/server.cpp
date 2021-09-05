#include "server.hpp"
#include <chrono>
#include <span>
#include "messages.hpp"
#include <glm/gtc/random.hpp>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "repl_sink.hpp"
#include "utils/logging.hpp"

using namespace std;

namespace Enflopio
{
    //Idk, repl might live more than a server,
    //but input will be stoped in normal termination
    Server::Server(const Options& options)
        : m_connection_manager(options.tcp_port, options.websocket_port),
          m_repl(std::make_shared<Repl>(*this))
    {

    }

    void Server::SetupLogging()
    {
        auto repl_sink = std::make_shared<repl_sink_mt>(m_repl);
        auto frame = std::make_shared<spdlog::logger>("frame", repl_sink);
        auto network = std::make_shared<spdlog::logger>("network", repl_sink);

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

        spdlog::default_logger()->sinks().clear();
        spdlog::default_logger()->sinks().push_back(repl_sink);

        spdlog::register_logger(frame);
        spdlog::register_logger(network);

        logging::frame = frame;
        logging::net = network;
    }

    void Server::Init()
    {
        m_repl->StartInput();

        SetupLogging();
        spdlog::info("Logging set");

        m_connection_manager.StartListening();

        //TODO Read config, get world size, etc

        for (int i = 0; i < 100; i++)
        {
            World::Circle n = {
                .position = glm::linearRand(glm::vec2(0, 0), m_world.GetSize())
            };

            m_world.AddCircle(n);
        }
    }

    void Server::Terminate()
    {
        m_connection_manager.StopListening();

        m_repl->StopInput();
    }

    void Server::PreSimulate(double delta)
    {
        ProcessConnections();
        ProcessMessages();
        logging::frame->info("PreSimulate");
        m_repl->ProcessCommands();
    }

    void Server::ProcessConnections()
    {
        m_connection_manager.ProcessConnections(
            [this](Connection::Ptr ptr)
            {
                NewConnect(std::move(ptr));
            },
            [this](Connection::Ptr ptr)
            {
                Disconnect(std::move(ptr));
            });
    }

    void Server::ProcessMessages()
    {
        for (auto& [connection, protocol] : m_connections)
        {
            while(connection->HasNext())
            {
                logging::net->debug("New message");
                auto message =
                    ServerMessages::Deserialize(connection->ReadNext());
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
                logging::net->debug("{}: ({}, {})",
                             id, player.position.x, player.position.y);
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
        logging::frame->info("PostSimulate");
        if (GetSimulationTick() % 4 == 0)
        {
            SendSync();
        }
    }

    void Server::NewConnect(Connection::Ptr connection)
    {
        logging::net->info("New connection");
        ProtocolImpl protocol(*connection, m_world);
        m_connections.insert(std::make_pair(std::move(connection),
                                            std::move(protocol)));
    }

    void Server::Disconnect(Connection::Ptr connection)
    {
        //TODO Add name
        logging::net->debug("Disconnect");
        auto it = m_connections.find(connection);
        it->second.Disconnect();
        m_connections.erase(connection);
    }
}
