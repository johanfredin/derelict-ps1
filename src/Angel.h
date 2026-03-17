#ifndef DERELICT_PS1_ANGEL_H
#define DERELICT_PS1_ANGEL_H
#include "psyqo/fragments.hh"
#include "psyqo/gpu.hh"
#include "psyqo/trigonometry.hh"
#include "psyqo/primitives/quads.hh"


class Angel {
public:
    static constexpr unsigned N_VERTICES = 8;
    static constexpr unsigned N_FACES = 6;

    // Represents one face of a quad. 1 face and 4 vertices
    struct Face {
        uint8_t vertices[4];
        psyqo::Color color;
    };

    void init();
    void draw(const psyqo::GPU &gpu);
private:
    eastl::array<psyqo::Fragments::SimpleFragment<psyqo::Prim::Quad>, N_FACES> m_quads;
    eastl::array<psyqo::Vec3, N_VERTICES> m_vertices;
    eastl::array<Face, N_FACES> m_cubeFaces{};
    psyqo::Angle m_rot;
    psyqo::Trig<> m_trig;
};

#endif //DERELICT_PS1_ANGEL_H