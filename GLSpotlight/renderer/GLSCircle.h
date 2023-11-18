#pragma once

#include <cstdlib>
#include <ctime>

#include "GLSRenderObject.h"

#define CIRCULAR_PRECISION 90

typedef float* circle_range;

class GLSCircle : GLSRenderObject
{
     circle_range radius_data;
     screen_pos circle_center;
     float max_radius;
     bool locked;

     void get_point_pos(float theta, float radius, screen_pos* pos);
     
public:
     GLSCircle() : max_radius(100.f), locked(false)
     {
         radius_data = new float[CIRCULAR_PRECISION]{0};
         set_random_radius();
         
         circle_center = glm::vec4(1280/2, 720/2, 0, 0);
         windowsize = new int[2];
         srand(time(NULL));
     }
    
    void render() override;

    void set_random_radius();

    void set_radius(float* radiuss)
    {
        radius_data = radiuss;
    }
};
