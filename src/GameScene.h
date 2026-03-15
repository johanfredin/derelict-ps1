#ifndef DERELICT_PS1_GAMESCENE_H
#define DERELICT_PS1_GAMESCENE_H
#include "Angel.h"
#include "psyqo/scene.hh"

class GameScene : public psyqo::Scene {
public:
    void start(StartReason reason) override;
    void frame() override;
private:
    Angel m_angel;
};


#endif //DERELICT_PS1_GAMESCENE_H
