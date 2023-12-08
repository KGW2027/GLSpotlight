#pragma once
#include <vector>

#include "StageObject.h"


struct RoomWall
{
    quad        points  ;
    Material    material;
};

class StageRoom : public StageObject
{

    std::vector<RoomWall> walls; 

    void draw_high_resolution(quad points);

protected:
    void pre_render() override;
    void rendering() override;
    void post_render() override;
public:
    
    void ready() override;
};
