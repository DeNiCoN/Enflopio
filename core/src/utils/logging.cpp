#include "logging.hpp"

namespace Enflopio
{
    namespace logging
    {
        std::shared_ptr<spdlog::logger> net = spdlog::default_logger();
        std::shared_ptr<spdlog::logger> frame = spdlog::default_logger();
        std::shared_ptr<spdlog::logger> render = spdlog::default_logger();
    }
}
