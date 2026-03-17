#ifndef DERELICT_PS1_DERELICT_H
#define DERELICT_PS1_DERELICT_H

#include "psyqo/application.hh"
#include "psyqo/font.hh"
#include "psyqo/fragments.hh"
#include "psyqo/matrix.hh"
#include "psyqo/trigonometry.hh"
#include "psyqo/primitives/quads.hh"

using namespace psyqo::fixed_point_literals;

class DerelictApplication : public psyqo::Application {
public:
    static constexpr unsigned OT_SIZE = 4096;
    static constexpr unsigned NUM_BUFFERS = 2;

    struct FrameBuf {
        psyqo::Fragments::SimpleFragment<psyqo::Prim::FastFill> clear;
        psyqo::OrderingTable<OT_SIZE> ot;
    };

    static constexpr psyqo::Matrix33 identifyMatrix = {
        {
            {1.0_fp, 0.0_fp, 0.0_fp},
            {0.0_fp, 1.0_fp, 0.0_fp},
            {0.0_fp, 0.0_fp, 1.0_fp},
        }
    };

    struct Fragment {
        uint32_t head{};
        psyqo::Prim::TexturedQuad quads[2];

        [[nodiscard]] size_t getActualFragmentSize() const {
            return sizeof(quads) / sizeof(uint32_t);
        }
    };

    static psyqo::Font<> font;
    static Fragment fragment;
    static FrameBuf frameBuf[2];
    void prepare() override;
    void start() override;
    void createScene() override;

private:
    psyqo::Trig<> trig;
};

#endif //DERELICT_PS1_DERELICT_H
