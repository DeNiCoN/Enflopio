#pragma once

#include <cereal/cereal.hpp>
#include <glm/glm.hpp>

namespace glm
{
template<class Archive>
void serialize(Archive& archive,
               glm::vec2& m)
{
    archive(m.x, m.y);
}
}
