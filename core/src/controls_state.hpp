#pragma once
#include <glm/vec2.hpp>
#include "serialization.hpp"
#include <tuple>

namespace Enflopio
{
    struct ControlsState
    {
        glm::vec2 direction;
    };

    inline bool operator==(const ControlsState& lhs, const ControlsState& rhs)
    {
        return std::tie(lhs.direction) == std::tie(rhs.direction);
    }

    template <typename Archive>
    void serialize(Archive& ar, ControlsState& state)
    {
        ar(state.direction);
    }
}
