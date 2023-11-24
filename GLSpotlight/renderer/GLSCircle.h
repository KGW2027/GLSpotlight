#pragma once

#include <chrono>
#include <cstdlib>
#include <ctime>

#include "GLSRenderObject.h"
#include "../music/MusicReader.h"

#define CIRCULAR_PRECISION 360

typedef double* circle_range;

class GLSCircle : GLSRenderObject
{
     double**       freq_info_;
     circle_range   radius_data_;
     screen_pos     circle_center_;
     float          max_radius_;
     float          avg_db_;
     MusicReader*   music_reader_;
     UINT32         play_idx_;
     UINT64         play_time_;

     void get_point_pos(float theta, float radius, screen_pos* pos);

    void set_hue_based_color(int idx);
     
public:
     GLSCircle() : max_radius_(200.f)
     {
         radius_data_ = new double[CIRCULAR_PRECISION]{max_radius_};
         circle_center_ = glm::vec4(1280/2, 720/2, 0, 0);
         windowsize = new int[2];
         srand(time(NULL));
     }
    
    void render() override;

    void set_random_radius();

    void set_radius(double* radiuss, UINT32* window_size);

    void play();

    void set_music_reader(MusicReader* reader)
    {
        music_reader_ = reader;
    }
};
