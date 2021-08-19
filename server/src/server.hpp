#pragma once
#include <cstdint>
#include <chrono>
#include "connection.hpp"
#include "tcp_connection_listener.hpp"
#include "websocket_connection_listener.hpp"
#include "protocol_impl.hpp"
#include "world.hpp"

namespace Enflopio
{
    class Server
    {
    public:
        struct Options;
        explicit Server(const Options& options);

        void Run();

        struct Options
        {
            std::uint16_t tcp_port = 25325;
            std::uint16_t websocket_port = 25326;
        };

        unsigned long GetTick() const { return m_tick; }

        void PendingConnect(Connection::Ptr);
        void PendingDisconnect(Connection::Ptr);
    private:
        void StartListening();
        void StopListening();

        void NewConnect(Connection::Ptr);
        void Disconnect(Connection::Ptr);

        void Frame();
        void ProcessMessages();
        void ProcessConnections();
        void SendSync();

        void InitClock();

        void Init();
        void Terminate();

        double UpdateClock();

        void UpdateGame(double delta);
        bool ShouldClose() const { return false; }

        using TimePoint = std::chrono::high_resolution_clock::time_point;
        using Duration = std::chrono::high_resolution_clock::duration;
        TimePoint m_last_update;
        Duration m_lag;
        Duration m_frame = std::chrono::duration_cast<Duration>(std::chrono::duration<double>(1.0/60.0));
        TimePoint m_current_update;

        unsigned long m_tick = 0;

        std::unordered_map<Connection::Ptr, ProtocolImpl> m_connections;
        std::mutex m_connections_mutex;
        std::vector<Connection::Ptr> m_pending_connect;
        std::vector<Connection::Ptr> m_pending_disconnect;
        TCPConnectionListener m_tcp_listener;
        WebSocketConnectionListener m_websocket_listener;

        World m_world;
    };
}
