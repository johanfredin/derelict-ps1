#include "GameScene.h"
#include "psyqo/primitives/quads.hh"
#include "psyqo/font.hh"


struct Fragment {
    uint32_t head{};
    psyqo::Prim::TexturedQuad quads[2];
    [[nodiscard]] size_t getActualFragmentSize() const {
        return sizeof(quads) / sizeof(uint32_t);
    }
};

Fragment frag;


void initBg() {
    psyqo::Prim::TexturedQuad bg_chunk1;
    psyqo::Prim::TexturedQuad bg_chunk2;
    bg_chunk1.pointA = {{.x = 0, .y = 0}};
    bg_chunk1.pointB = {{.x = 256, .y = 0}};
    bg_chunk1.pointC = {{.x = 0, .y = 240}};
    bg_chunk1.pointD = {{.x = 256, .y = 240}};
    bg_chunk1.uvA = {.u = 0, .v = 0};
    bg_chunk1.uvB = {.u = 255, .v = 0};
    bg_chunk1.uvC = {.u = 0, .v = 239};
    bg_chunk1.uvD = {.u = 255, .v = 239};
    bg_chunk1.tpage
            .set(psyqo::Prim::TPageAttr::ColorMode::Tex16Bits)
            .setPageX(5)
            .setPageY(0);

    bg_chunk2.pointA = {{.x = 256, .y = 0}};
    bg_chunk2.pointB = {{.x = 320, .y = 0}};
    bg_chunk2.pointC = {{.x = 256, .y = 240}};
    bg_chunk2.pointD = {{.x = 320, .y = 240}};
    bg_chunk2.uvA = {.u = 0, .v = 0};
    bg_chunk2.uvB = {.u = 63, .v = 0};
    bg_chunk2.uvC = {.u = 0, .v = 239};
    bg_chunk2.uvD = {.u = 63, .v = 239};
    bg_chunk2.tpage
            .set(psyqo::Prim::TPageAttr::ColorMode::Tex16Bits)
            .setPageX(9)
            .setPageY(0);

    frag.quads[0] = bg_chunk1;
    frag.quads[1] = bg_chunk2;
}

void GameScene::start(const StartReason reason) {
    initBg();
    m_angel.init();
    Scene::start(reason);
}

void GameScene::frame() {
    constexpr psyqo::Color c = {10, 10, 10};
    gpu().clear(c);
    gpu().sendFragment(frag);
    // m_angel.draw(gpu());

}
