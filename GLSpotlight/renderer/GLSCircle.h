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
     float min_radius;

     void get_point_pos(float theta, float radius, screen_pos* pos);
     
public:
     GLSCircle() : min_radius(100.f)
     {
         radius_data = new float[CIRCULAR_PRECISION]{0};
         
         circle_center = new float[3]{0, 0, 0};
         windowsize = new int[2];
     }
    
    void render() override;

    void set_random_radius()
    {
        srand(time(NULL));
        for(int i = 0 ; i < CIRCULAR_PRECISION ; i++)
            radius_data[i] = rand() % 200 + 50;
    }

    void set_radius(float* radiuss)
    {
        radius_data = radiuss;
    }
};
