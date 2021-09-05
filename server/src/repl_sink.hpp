#pragma once

#include <spdlog/sinks/base_sink.h>
#include <spdlog/details/null_mutex.h>
#include <mutex>
#include <iostream>
#include "repl.hpp"

namespace Enflopio
{

    template<typename Mutex>
    class repl_sink : public spdlog::sinks::base_sink<Mutex>
    {
    public:
        repl_sink(std::weak_ptr<Repl> repl)
            : m_repl(std::move(repl))
        {}
    protected:
        void sink_it_(const spdlog::details::log_msg& msg) override
        {

            spdlog::memory_buf_t formatted;
            spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);

            if (!m_repl.expired())
            {
                m_repl.lock()->Write(fmt::to_string(formatted));
            }
            else
            {
                std::cerr << fmt::to_string(formatted);
            }
        }

        void flush_() override
        {

        }

        std::weak_ptr<Repl> m_repl;
    };
    using repl_sink_mt = repl_sink<std::mutex>;
    using repl_sink_st = repl_sink<spdlog::details::null_mutex>;
}
