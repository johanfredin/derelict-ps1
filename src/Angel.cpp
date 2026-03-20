#include "Angel.h"

#include "DerelictApplication.h"
#include "psyqo/gte-registers.hh"
#include "psyqo/fixed-point.hh"
#include "psyqo/soft-math.hh"
#include "psyqo/primitives/quads.hh"
#include "psyqo/primitives/lines.hh"


namespace GTE = psyqo::GTE;
using Register = GTE::Register;

using namespace psyqo::fixed_point_literals;
using namespace psyqo::trig_literals;

static constexpr psyqo::Color FONT_COLOR = {.r=0xFF, .g=0xFF, .b=0xFF};
static constexpr  uint8_t FACE_NORMAL = 0;
static constexpr psyqo::Color COLOR_NORMAL = {.r = 255, .g = 0, .b = 0};

void Angel::init(psyqo::GPU &gpu) {
    m_font.uploadSystemFont(gpu);
    // Init da cube
    m_cubeVertices = {
        {
            // Front verts
            {.x = -0.025, .y = -0.025, .z = -0.025},
            {.x = 0.025, .y = -0.025, .z = -0.025},
            {.x = -0.025, .y = 0.025, .z = -0.025},
            {.x = 0.025, .y = 0.025, .z = -0.025},
            // Back verts
            {.x = -0.025, .y = -0.025, .z = 0.025},
            {.x = 0.025, .y = -0.025, .z = 0.025},
            {.x = -0.025, .y = 0.025, .z = 0.025},
            {.x = 0.025, .y = 0.025, .z = 0.025}
        }
    };

    // Center no?
    m_cubeNormal = {
        {
            {.x = 0, .y = 0, .z = -0.025},
            {.x = 0, .y = 0, .z = -0.050}
        }
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

void Angel::draw(psyqo::GPU &gpu) {
    eastl::array<psyqo::Vertex, 4> projectedCube{};
    eastl::array<psyqo::Vertex, 2> projectedNormal{};

    // We want the cube to appear slightly further away, so we translate it by 512 on the Z-axis.
    psyqo::GTE::write<Register::TRZ, GTE::Unsafe>(512);

    // Generate rotation matrices for x any y axes (skipping z for now)
    psyqo::Matrix33 transform = psyqo::SoftMath::generateRotationMatrix33(0, psyqo::SoftMath::Axis::X, m_trig);
    psyqo::Matrix33 rot = psyqo::SoftMath::generateRotationMatrix33(m_rot, psyqo::SoftMath::Axis::Y, m_trig);

    // Z axis rotation
    // psyqo::SoftMath::generateRotationMatrix33(&rot, m_rot, psyqo::SoftMath::Axis::Z, m_trig);

    // Multiply x y z matrices
    psyqo::SoftMath::multiplyMatrix33(transform, rot, &transform);

    // Apply the combined rotation and write it to the pseudo register for the cube's rotation
    GTE::writeUnsafe<GTE::PseudoRegister::Rotation>(transform);

    int faceNum = 0;
    auto& ot = DerelictApplication::frameBuf[gpu.getParity()].ot;
    for (auto face : m_cubeFaces) {
        // Load first 3 vertices into gte
        GTE::writeUnsafe<GTE::PseudoRegister::V0>(m_cubeVertices[face.vertices[0]]);
        GTE::writeUnsafe<GTE::PseudoRegister::V1>(m_cubeVertices[face.vertices[1]]);
        GTE::writeSafe<GTE::PseudoRegister::V2>(m_cubeVertices[face.vertices[2]]);

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
        GTE::read<Register::SXY0>(&projectedCube[0].packed);

        // Now override and perform projection on the last vertex
        GTE::writeSafe<GTE::PseudoRegister::V0>(m_cubeVertices[face.vertices[3]]);
        GTE::Kernels::rtps();

        // Calculate avg z value for OT
        int32_t zIndex = 0;
        GTE::Kernels::avsz4();
        GTE::read<Register::OTZ>(reinterpret_cast<uint32_t*>(&zIndex));

        if (zIndex < 0 || zIndex >= DerelictApplication::OT_SIZE) {
            continue;
        }

        // Read the remaining 3 projected vertices
        GTE::read<Register::SXY0>(&projectedCube[1].packed);
        GTE::read<Register::SXY1>(&projectedCube[2].packed);
        GTE::read<Register::SXY2>(&projectedCube[3].packed);

        // Draw the cube face
        auto& quad = m_quads[faceNum];
        quad.primitive.setPointA(projectedCube[0]);
        quad.primitive.setPointB(projectedCube[1]);
        quad.primitive.setPointC(projectedCube[2]);
        quad.primitive.setPointD(projectedCube[3]);
        quad.primitive.setColor(face.color);
        quad.primitive.setOpaque();

        ot.insert(quad, zIndex);
        if (faceNum == FACE_NORMAL) {
            // Compute face center in object space
            // const auto& v0 = m_cubeVertices[face.vertices[0]];
            // const auto& v1 = m_cubeVertices[face.vertices[1]];
            // const auto& v2 = m_cubeVertices[face.vertices[2]];
            // const auto& v3 = m_cubeVertices[face.vertices[3]];
            //
            // psyqo::Vec3 center = {
            //     .x = (v0.x + v1.x + v2.x + v3.x) / 4,
            //     .y = (v0.y + v1.y + v2.y + v3.y) / 4,
            //     .z = (v0.z + v1.z + v2.z + v3.z) / 4
            // };
            //
            // // A short line along the face's local normal direction.
            // // For your current chosen face, this points along -Z.
            // psyqo::Vec3 normalEnd = center;
            // normalEnd.z -= 0.025;
            //
            // // Project both endpoints separately
            // GTE::writeUnsafe<GTE::PseudoRegister::V0>(center);
            // GTE::writeUnsafe<GTE::PseudoRegister::V1>(normalEnd);
            // GTE::Kernels::rtpt();
            // GTE::read<Register::SXY0>(&projectedNormal[0].packed);
            // GTE::read<Register::SXY1>(&projectedNormal[1].packed);
            //
            // m_line.primitive.pointA = projectedNormal[0];
            // m_line.primitive.pointB = projectedNormal[1];
            // m_line.primitive.setColor(COLOR_NORMAL);
            //
            // ot.insert(m_line, zIndex);

            //
            GTE::writeUnsafe<GTE::PseudoRegister::V0>(m_cubeNormal[0]);
            GTE::writeUnsafe<GTE::PseudoRegister::V1>(m_cubeNormal[1]);
            GTE::Kernels::rtpt();
            GTE::read<Register::SXY0>(&projectedNormal[1].packed);
            GTE::read<Register::SXY1>(&projectedNormal[0].packed);
            m_line.primitive.pointA = projectedNormal[0];
            m_line.primitive.pointB = projectedNormal[1];

            m_line.primitive.setColor(COLOR_NORMAL);
            ot.insert(m_line, zIndex);
        }


        faceNum++;
    }
    // m_font.print(gpu, "YO", {100, 100}, FONT_COLOR);
    // m_rot += 0.005_pi;
}

void Angel::tick() {
    if (DerelictApplication::isPad1Btn(psyqo::SimplePad::Left)) {
        m_rot -= 0.008_pi;
    } else if (DerelictApplication::isPad1Btn(psyqo::SimplePad::Right)) {
        m_rot += 0.008_pi;
    }
}
