#include "Angel.h"

#include "DerelictApplication.h"
#include "GameScene.h"
#include "psyqo/gte-registers.hh"
#include "psyqo/fixed-point.hh"
#include "psyqo/soft-math.hh"
#include "psyqo/primitives/quads.hh"
#include "psyqo/primitives/lines.hh"

namespace GTE = psyqo::GTE;
using Register = GTE::Register;

using namespace psyqo::fixed_point_literals;
using namespace psyqo::trig_literals;

static constexpr uint8_t FACE_FRONT = 0;

void Angel::init() {
    // Init da cube
    m_vertices = {
        {
            {.x = -0.025, .y = -0.025, .z = -0.025},
            {.x = 0.025, .y = -0.025, .z = -0.025},
            {.x = -0.025, .y = 0.025, .z = -0.025},
            {.x = 0.025, .y = 0.025, .z = -0.025},
            {.x = -0.025, .y = -0.025, .z = 0.025},
            {.x = 0.025, .y = -0.025, .z = 0.025},
            {.x = -0.025, .y = 0.025, .z = 0.025},
            {.x = 0.025, .y = 0.025, .z = 0.025}
        }
    };

    const psyqo::Vec3 front[4] = {
        m_vertices[0], m_vertices[1], m_vertices[2], m_vertices[3]
    };

    m_cubeFaces = {{
        {{0, 1, 2, 3}, {0, 0, 255}},
        {{6, 7, 4, 5}, {0, 255, 0}},
        {{4, 5, 0, 1}, {0, 255, 255}},
        {{7, 6, 3, 2}, {255, 0, 0}},
        {{6, 4, 2, 0}, {255, 0, 255}},
        {{5, 7, 1, 3}, {255, 255, 0}}
    }};

}

void Angel::draw(const psyqo::GPU &gpu) {
    eastl::array<psyqo::Vertex, 4> projected{};

    // We want the cube to appear slightly further away, so we translate it by 512 on the Z-axis.
    psyqo::GTE::write<Register::TRZ, GTE::Unsafe>(512);

    // Generate rotation matrices for x any y axes (skipping z for now)
    psyqo::Matrix33 transform = psyqo::SoftMath::generateRotationMatrix33(m_rot, psyqo::SoftMath::Axis::X, m_trig);
    psyqo::Matrix33 rot = psyqo::SoftMath::generateRotationMatrix33(m_rot, psyqo::SoftMath::Axis::Y, m_trig);

    // Z axis rotation
    psyqo::SoftMath::generateRotationMatrix33(&rot, m_rot, psyqo::SoftMath::Axis::Z, m_trig);

    // Multiply x y z matrices
    psyqo::SoftMath::multiplyMatrix33(transform, rot, &transform);



    // Apply the combined rotation and write it to the pseudo register for the cube's rotation
    GTE::writeUnsafe<GTE::PseudoRegister::Rotation>(transform);

    int faceNum = 0;
    auto& ot = DerelictApplication::frameBuf[gpu.getParity()].ot;
    for (auto face : m_cubeFaces) {
        // Load first 3 vertices into gte
        GTE::writeUnsafe<GTE::PseudoRegister::V0>(m_vertices[face.vertices[0]]);
        GTE::writeUnsafe<GTE::PseudoRegister::V1>(m_vertices[face.vertices[1]]);
        GTE::writeSafe<GTE::PseudoRegister::V2>(m_vertices[face.vertices[2]]);

        // Perform perspective transform on the 3 vertices
        GTE::Kernels::rtpt();

        // Perform normal clipping to determine if this face should be rendered
        GTE::Kernels::nclip();
        int32_t nclip = 0;
        GTE::read<Register::MAC0>(reinterpret_cast<uint32_t*>(&nclip));
        if (nclip <= 0) {
            continue;
        }

        // Store our first projected (since we will overwrite the register with the 4h vertex)
        GTE::read<Register::SXY0>(&projected[0].packed);

        // Now override and perform projection on the last vertex
        GTE::writeSafe<GTE::PseudoRegister::V0>(m_vertices[face.vertices[3]]);
        GTE::Kernels::rtps();

        // Calculate avg z value for OT
        int32_t zIndex = 0;
        GTE::Kernels::avsz4();
        GTE::read<Register::OTZ>(reinterpret_cast<uint32_t*>(&zIndex));

        if (zIndex < 0 || zIndex >= DerelictApplication::OT_SIZE) {
            continue;
        }

        // Read the remaining 3 projected vertices
        GTE::read<Register::SXY0>(&projected[1].packed);
        GTE::read<Register::SXY1>(&projected[2].packed);
        GTE::read<Register::SXY2>(&projected[3].packed);

        // Draw the cube face
        auto& quad = m_quads[faceNum];
        quad.primitive.setPointA(projected[0]);
        quad.primitive.setPointB(projected[1]);
        quad.primitive.setPointC(projected[2]);
        quad.primitive.setPointD(projected[3]);
        quad.primitive.setColor(face.color);
        quad.primitive.setOpaque();

        ot.insert(quad, zIndex);
        faceNum++;
    }

    m_rot += 0.005_pi;
}
