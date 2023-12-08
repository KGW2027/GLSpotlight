#pragma once
#include <vector>

#include "StageObject.h"

typedef glm::vec3* quad;

struct RoomWall
{
    quad        points  ;
    Material    material;
};

class StageRoom : public StageObject
{

    std::vector<RoomWall> walls; 
    quad make_quad(float x1, float y1, float z1, float x2, float y2, float z2);

protected:
    void pre_render() override;
    void rendering() override;
    void post_render() override;
public:
    
    void ready() override;
};
