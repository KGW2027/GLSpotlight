#pragma once

#include "StageObject.h"

class StageSpotlight : public StageObject
{
    LightSource         light_data_   ;
    float               yaw_offset_   ;
    float               yaw_direction_;
    
    void pre_render() override;
    void rendering() override;
    void post_render() override;

    void calc_direction();
public:
    StageSpotlight(GLenum light_source);

    void ready() override;
};
