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
    /* External Data */
    double**        freq_info_;
    MusicReader*    music_reader_;

    /* Draw Data */
    circle_range    radius_data_;
    screen_pos      circle_center_;
    float           max_radius_;
    uint            rad_len;
    
    void add_data(uint idx, double value);
    void draw_frame(double* data, UINT32* window_size);
    bool is_muted(double value);

    /* Debug Data*/
    float           avg_db_;
    UINT32          play_idx_;
    UINT64          play_time_;
    
    void get_point_pos(float theta, float radius, screen_pos* pos);
    void set_hue_based_color(int idx);
     
public:
    GLSCircle(int radius_size) : max_radius_(200.f)
    {
        rad_len = radius_size;
        radius_data_ = new double[radius_size]{0};
        circle_center_ = glm::vec4(1280/2, 720/2, 0, 0);
        windowsize = new int[2];
        srand(time(NULL));
    }
    
    void render() override;

    void set_random_radius();

    void set_radius(double* prev, double* radiuss, UINT32* window_size);

    void play();

    void set_music_reader(MusicReader* reader)
    {
        music_reader_ = reader;
    }
};
