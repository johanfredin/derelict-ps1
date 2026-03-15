#ifndef DERELICT_PS1_DERELICT_H
#define DERELICT_PS1_DERELICT_H
#include "psyqo/application.hh"
#include "psyqo/matrix.hh"
#include "psyqo/trigonometry.hh"

using namespace psyqo::fixed_point_literals;

class DerelictApplication : public psyqo::Application {
public:
    static constexpr psyqo::Matrix33 identifyMatrix = {
            {
                {1.0_fp, 0.0_fp, 0.0_fp},
                {0.0_fp, 1.0_fp, 0.0_fp},
                {0.0_fp, 0.0_fp, 1.0_fp},
            }
        };

    void prepare() override;
    void start() override;
    void createScene() override;
private:
    psyqo::Trig<> trig;
};


#endif //DERELICT_PS1_DERELICT_H