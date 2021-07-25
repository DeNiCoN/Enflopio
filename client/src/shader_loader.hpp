#pragma once
#include "Shader.hpp"
#include <string_view>

namespace Enflopio
{
    class ShaderLoader
    {
    public:
        static Shader FromText(std::string_view vertex_text, std::string_view fragment_text);
    };
}
