#pragma once

#include "smol/core/component.h"
#include "smol/defines.h"

namespace smol_game
{
    class SMOL_API test_component_t : public smol::core::component_t
    {
      public:
        void start();
        void update(f64 delta_time);
    };
} // namespace smol_game