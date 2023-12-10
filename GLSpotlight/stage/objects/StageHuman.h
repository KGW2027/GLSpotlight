#pragma once
#include "StageObject.h"

enum LIGHT_MOVE_MODE
{
    FWD  = 0x01,
    SIDE = 0x02,
};

class StageHuman : public StageObject
{
    Material mat_head_  ;
    Material mat_stick_ ;

    float           interp_point_ ;
    float           arm_deg_      ;
    bool            arm_ascend_   ;
    LIGHT_MOVE_MODE arm_mode_     ;

    vec3            cur_color_    ;
    vec3            target_color_ ;
    unsigned char   color_change_ ;

    void     move_arm();
    GLfloat* random_color(int i);
protected:
    void pre_render() override;
    void rendering() override;
    void post_render() override;
public:

    StageHuman(float x, float z);
    
    void ready() override;
    
};
