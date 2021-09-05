#pragma once
#include <spdlog/spdlog.h>

namespace Enflopio
{
    namespace logging
    {
        extern std::shared_ptr<spdlog::logger> net;
        extern std::shared_ptr<spdlog::logger> frame;
        extern std::shared_ptr<spdlog::logger> render;
    }
}
