#include "Angel.h"

#include "psyqo/gte-registers.hh"
#include "psyqo/fixed-point.hh"
#include "psyqo/soft-math.hh"
#include "psyqo/primitives/quads.hh"

namespace GTE = psyqo::GTE;
using Register = GTE::Register;

using namespace psyqo::fixed_point_literals;
using namespace psyqo::trig_literals;

static constexpr psyqo::Matrix33 identity = {{
    {1.0_fp, 0.0_fp, 0.0_fp},
    {0.0_fp, 1.0_fp, 0.0_fp},
    {0.0_fp, 0.0_fp, 1.0_fp},
}};

void Angel::init() {
    GTE::clear<Register::TRX, GTE::Unsafe>();
    GTE::clear<Register::TRY, GTE::Unsafe>();
    GTE::clear<Register::TRZ, GTE::Unsafe>();

    GTE::write<Register::OFX, GTE::Unsafe>(psyqo::FixedPoint<16>(160.0).raw());
    GTE::write<Register::OFY, GTE::Unsafe>(psyqo::FixedPoint<16>(120.0).raw());

    // Projection plane distance
    GTE::write<Register::H, GTE::Unsafe>(120);

    m_vertices[0] = {.x = -0.05_fp, .y = -0.05_fp, .z = 0.0_fp};
    m_vertices[1] = {.x =  0.05_fp, .y = -0.05_fp, .z = 0.0_fp};
    m_vertices[2] = {.x = -0.05_fp, .y =  0.05_fp, .z = 0.0_fp};
    m_vertices[3] = {.x =  0.05_fp, .y =  0.05_fp, .z = 0.0_fp};

    m_quad.setColor({255, 10, 50});
}

void Angel::draw(psyqo::GPU &gpu) {
    // Write identity matrix every frame - GTE registers are global hardware state
    GTE::writeSafe<GTE::PseudoRegister::Rotation>(identity);

    // rtpt projects 3 vertices at once into SXY0, SXY1, SXY2
    GTE::writeUnsafe<GTE::PseudoRegister::V0>(m_vertices[0]);
    GTE::writeUnsafe<GTE::PseudoRegister::V1>(m_vertices[1]);
    GTE::writeSafe<GTE::PseudoRegister::V2>  (m_vertices[2]);
    GTE::Kernels::rtpt();
    GTE::read<Register::SXY0>(&m_quad.pointA.packed);
    GTE::read<Register::SXY1>(&m_quad.pointB.packed);
    GTE::read<Register::SXY2>(&m_quad.pointC.packed);

    // rtps projects the last vertex into SXY2
    GTE::writeSafe<GTE::PseudoRegister::V0>(m_vertices[3]);
    GTE::Kernels::rtps();
    GTE::read<Register::SXY2>(&m_quad.pointD.packed);

    gpu.sendPrimitive(m_quad);
}
