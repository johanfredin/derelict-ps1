//
// Created by johan on 2026-03-15.
//

#ifndef DERELICT_PS1_ENGINE_H
#define DERELICT_PS1_ENGINE_H

#include "psyqo/fragments.hh"
#include "psyqo/trigonometry.hh"
#include "psyqo/fixed-point.hh"
#include "psyqo/gpu.hh"
#include "psyqo/matrix.hh"
#include "psyqo/primitives/quads.hh"

using namespace psyqo::fixed_point_literals;
using namespace psyqo::trig_literals;

namespace engine {
    // static constexpr unsigned OT_SIZE = 240;
    //
    // // We need to create 2 OrderingTable objects since we can't reuse a single one for both
    // // framebuffers, as the previous one may not finish transfering in time.
    // static const psyqo::OrderingTable<OT_SIZE> ots[2];
    //
    // // Since we're using an ordering table, we need to sort fill commands as well,
    // // otherwise they'll draw over our beautiful cube.
    // static const psyqo::Fragments::SimpleFragment<psyqo::Prim::FastFill> clear[2];
    //
    // struct gpu {
    //     struct Fragment {
    //         uint32_t head{};
    //         psyqo::Prim::TexturedQuad quads[2];
    //         [[nodiscard]] size_t getActualFragmentSize() const {
    //             return sizeof(quads) / sizeof(uint32_t);
    //         }
    //     };
    //
    //     static void init(psyqo::GPU &gpu);
    //     void otSort(Fragment &f);
    // };
    //
    // struct gte {
    //     static constexpr psyqo::Matrix33 identifyMatrix = {
    //         {
    //             {1.0_fp, 0.0_fp, 0.0_fp},
    //             {0.0_fp, 1.0_fp, 0.0_fp},
    //             {0.0_fp, 0.0_fp, 1.0_fp},
    //         }
    //     };
    //
    //     static void init();
    // };
}
#endif //DERELICT_PS1_ENGINE_H
