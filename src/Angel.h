#ifndef DERELICT_PS1_ANGEL_H
#define DERELICT_PS1_ANGEL_H
#include "psyqo/gpu.hh"
#include "psyqo/primitives/quads.hh"


class Angel {
public:
    void init();
    void draw(psyqo::GPU &gpu);
private:
    psyqo::Vec3 m_vertices[4];
    psyqo::Prim::Quad m_quad;
};

#endif //DERELICT_PS1_ANGEL_H