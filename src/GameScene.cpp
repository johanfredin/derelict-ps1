#include "GameScene.h"

#include "DerelictApplication.h"
#include "psyqo/primitives/quads.hh"

DerelictApplication::Fragment frag;

static constexpr psyqo::Color bgc = {.r = 100, .g = 100, .b = 100};
static constexpr psyqo::Color dark = {.r = 64, .g = 32, .b = 48};

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
    bg_chunk1.setColor(dark);

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
    bg_chunk2.setColor(dark);


    frag.quads[0] = bg_chunk1;
    frag.quads[1] = bg_chunk2;
}

void GameScene::start(const StartReason reason) {
    initBg();
    m_angel.init(gpu());
    Scene::start(reason);
}

void GameScene::frame() {
    m_angel.tick();
    const auto curFrame = gpu().getParity();
    auto&[clear, ot] = DerelictApplication::frameBuf[curFrame];

    ot.clear();
    gpu().getNextClear(clear.primitive, bgc);
    gpu().chain(clear);

    // Insert bg at the back
    ot.insert(frag, DerelictApplication::OT_SIZE - 1);

    m_angel.draw(gpu());

    gpu().chain(ot);
}
