#pragma once
#include <cstdint>
#include <chrono>
#include "connection.hpp"
#include "tcp_connection_listener.hpp"
#include "websocket_connection_listener.hpp"
#include "protocol_impl.hpp"
#include "world.hpp"
#include "game_loop.hpp"
#include "repl.hpp"

namespace Enflopio
{
    class Server : private GameLoop<Server>
    {
    public:
        using GameLoop<Server>::Run;
        friend class GameLoop<Server>;

        struct Options;
        explicit Server(const Options& options);

        struct Options
        {
            std::uint16_t tcp_port = 25325;
            std::uint16_t websocket_port = 25326;
        };

        void PendingConnect(Connection::Ptr);
        void PendingDisconnect(Connection::Ptr);
    private:
        static constexpr std::size_t NUM_BACKTRACE_LOG_MESSAGES = 128;

        void SetupLogging();
        void StartListening();
        void StopListening();

        void NewConnect(Connection::Ptr);
        void Disconnect(Connection::Ptr);

        void ProcessMessages();
        void ProcessConnections();
        void SendSync();

        void Init();
        void PreSimulate(double delta);
        void Simulate(double delta);
        void PostSimulate(double delta);
        void Terminate();
        bool ShouldStop() const { return m_should_stop; }
        bool ShouldSleep() const { return true; }

        bool m_should_stop = false;

        std::unordered_map<Connection::Ptr, ProtocolImpl> m_connections;
        std::mutex m_connections_mutex;
        std::vector<Connection::Ptr> m_pending_connect;
        std::vector<Connection::Ptr> m_pending_disconnect;
        TCPConnectionListener m_tcp_listener;
        WebSocketConnectionListener m_websocket_listener;

        std::shared_ptr<Repl> m_repl;
        World m_world;
    };
}
