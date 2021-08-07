#pragma once
#include <string>
#include <deque>
#include <mutex>
#include <thread>
#include <spdlog/spdlog.h>

#include <emscripten/websocket.h>

namespace Enflopio
{
    class WebsocketConnection
    {
    public:
        using Message = std::string;
        using MessageView = std::string_view;
        using Messages = std::deque<Message>;

        void Connect(const std::string& uri)
        {
            EmscriptenWebSocketCreateAttributes attributes;
            attributes.url = uri.c_str();
            emscripten_websocket_init_create_attributes(attributes);

            m_socket = emscripten_websocket_new(&attributes);

            if (m_socket <= 0)
            {
                spdlog::error("Websocket creation failed with error code {}", m_socket);
            }

            emscripten_websocket_set_onopen_callback(m_socket, static_cast<void*>(this), WebsocketOpen);
            emscripten_websocket_set_onclose_callback(m_socket, static_cast<void*>(this), WebsocketClose);
            emscripten_websocket_set_onerror_callback(m_socket, static_cast<void*>(this), WebsocketError);
            emscripten_websocket_set_onmessage_callback(m_socket, static_cast<void*>(this), WebsocketMessage);
        }

        void Send(Message message)
        {
            emscripten_websocket_send_binary(m_socket, message.data(), message.size());
        }

        Message NextMessage()
        {
            auto result = std::move(m_input_messages.front());
            m_messages.pop_front();
            return result;
        }

        bool HasNextMessage() const
        {
            return m_messages.size();
        }

    private:
        static EM_BOOL WebSocketOpen(int eventType,
                                     const EmscriptenWebSocketOpenEvent* e,
                                     void *userData)
        {
            spdlog::debug("WebSocket connection opened");
            return 0;
        }

        static EM_BOOL WebSocketClose(int eventType,
                               const EmscriptenWebSocketCloseEvent *e,
                               void *userData)
        {
            spdlog::error("close(eventType={}, wasClean={}, code={}, reason={}, userData={})\n",
                          eventType, e->wasClean, e->code, e->reason, (long)userData);
            return 0;
        }

        static EM_BOOL WebSocketError(int eventType,
                               const EmscriptenWebSocketErrorEvent *e,
                               void *userData)
        {
            spdlog::error("WebSocket error(eventType={}, userData={})\n",
                          eventType, (long)userData);
            return 0;
        }

        static EM_BOOL WebSocketMessage(int eventType,
                                 const EmscriptenWebSocketMessageEvent *e,
                                 void *userData)
        {
            auto* connection = static_cast<WebsocketConnection*>(userData);
            spdlog::debug(
                "message(eventType={}, userData={}, data={}, numBytes={}, isText={})\n",
                eventType, (long)userData, e->data, e->numBytes, e->isText);

            connection->m_messages.push_back(Message(static_cast<char*>(e->data), e->numBytes))

            return 0;
        }

        Messages m_messages;
        EMSCRIPTEN_WEBSOCKET_T m_socket;
    };
}