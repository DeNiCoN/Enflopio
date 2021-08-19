#pragma once
#include "controls_state.hpp"
#include <cereal/types/chrono.hpp>

namespace Enflopio
{
    struct NetworkControls
    {
        using Timestamp = std::chrono::high_resolution_clock::time_point;
        using ID = std::uint32_t;
        ControlsState state;
        ID id = 0;
        Timestamp timestamp = Timestamp::min();
        double delta = 0.;
    };

    template <typename Archive>
    void serialize(Archive& ar, NetworkControls& rhs)
    {
        ar(rhs.state, rhs.id, rhs.timestamp);
    }
}
