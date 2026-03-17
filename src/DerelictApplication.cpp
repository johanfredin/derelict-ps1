#include "DerelictApplication.h"

#include "GameScene.h"
#include "../assets/raw/angel.h"
#include "../assets/raw/bedroom_1.h"
#include "../assets/raw/bedroom_2.h"
#include "psyqo/gpu.hh"
#include "psyqo/primitives/quads.hh"

GameScene game_scene;

// Private helpers  -----------------------------------------
static constexpr psyqo::FixedPoint<16> GTE_ORIGIN_X(160.0);
static constexpr psyqo::FixedPoint<16> GTE_ORIGIN_Y(120.0);
static constexpr unsigned PROJECTION_PLANE_DISTANCE = 120;
static constexpr unsigned AVG_Z_TRI = DerelictApplication::OT_SIZE / 3;
static constexpr unsigned AVG_Z_QUAD = DerelictApplication::OT_SIZE / 4;

// So that we ACTUALLY get static shared instances
DerelictApplication::Fragment DerelictApplication::fragment;
DerelictApplication::FrameBuf DerelictApplication::frameBuf[2];
psyqo::Font<> DerelictApplication::font;

static void initGpu(psyqo::GPU &gpu, const psyqo::GPU::Resolution res, const psyqo::GPU::Interlace interlace) {
    psyqo::GPU::Configuration config;
    config
            .set(res)
            .set(psyqo::GPU::VideoMode::AUTO)
            .set(psyqo::GPU::ColorMode::C15BITS)
            .set(interlace);
    gpu.initialize(config);
}

static void initGte() {
    // Clear translation registers
    psyqo::GTE::clear<psyqo::GTE::Register::TRX, psyqo::GTE::Unsafe>();
    psyqo::GTE::clear<psyqo::GTE::Register::TRY, psyqo::GTE::Unsafe>();
    psyqo::GTE::clear<psyqo::GTE::Register::TRZ, psyqo::GTE::Unsafe>();

    // Set screen offset in the GTE
    psyqo::GTE::write<psyqo::GTE::Register::OFX, psyqo::GTE::Unsafe>(GTE_ORIGIN_X.raw());
    psyqo::GTE::write<psyqo::GTE::Register::OFY, psyqo::GTE::Unsafe>(GTE_ORIGIN_Y.raw());

    // Write projection plane distance
    psyqo::GTE::write<psyqo::GTE::Register::H, psyqo::GTE::Unsafe>(PROJECTION_PLANE_DISTANCE);

    // Set scaling for Z averaging
    psyqo::GTE::write<psyqo::GTE::Register::ZSF3, psyqo::GTE::Unsafe>(AVG_Z_TRI);
    psyqo::GTE::write<psyqo::GTE::Register::ZSF4, psyqo::GTE::Unsafe>(AVG_Z_QUAD);

}

static void loadToVRAM(psyqo::GPU &gpu, void *data, uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    psyqo::Rect r{};
    r.pos.x = x;
    r.pos.y = y;
    r.size.w = w;
    r.size.h = h;
    gpu.uploadToVRAM(static_cast<uint16_t *>(data), r);
}

// ------------------------------------------------------


void DerelictApplication::prepare() {
    initGpu(gpu(),
            psyqo::GPU::Resolution::W320,
            psyqo::GPU::Interlace::PROGRESSIVE
    );
    initGte();
}

void DerelictApplication::createScene() {
    pushScene(&game_scene);
}

void DerelictApplication::start() {
    font.uploadSystemFont(gpu());
    loadToVRAM(gpu(), assets_tim_bedroom_1_tim, 320, 0, 320, 240);
    loadToVRAM(gpu(), assets_tim_bedroom_2_tim, 320 * 2, 0, 320, 240);
    loadToVRAM(gpu(), angel_tim, 320, 256, 80, 200);
}


