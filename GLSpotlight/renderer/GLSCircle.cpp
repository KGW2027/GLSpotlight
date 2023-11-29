#include "GLSCircle.h"

#include <mfreadwrite.h>
#include <windows.h>
#include <string>
#include <thread>
#include <gl/freeglut.h>
#include <GL/gl.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define M_E 2.7182818284590452354
#define M_PI 3.14159265358979323846

#define INTERP_RANGE 1
#define DECAY 0.9
#define MIN_dB 80.0


void GLSCircle::add_data(uint pos, double value)
{
    for(uint idx = max(0, pos - INTERP_RANGE) ; idx < min(rad_len, pos + INTERP_RANGE) ; idx++)
        radius_data_[idx] = value;
}

void GLSCircle::draw_frame(double* data, UINT32* window_size)
{
    for(uint idx = 0 ; idx < *window_size ; idx++)
    {
        if(is_muted(data[idx])) radius_data_[idx] *= DECAY;
        else add_data(idx, data[idx]);
    }
}

bool GLSCircle::is_muted(double value)
{
    return abs(MIN_dB + value) < 0.01;
}

void GLSCircle::get_point_pos(float theta, float radius, screen_pos* pos)
{
    glm::vec4 dir_vec = glm::vec4(radius, 0, 0, 1);
    glm::mat4 rot = rotate(glm::mat4(1), glm::radians(theta), glm::vec3(0, 0, 1));
    *pos = circle_center_ + dir_vec * rot;
    glut_pos_to_gl_pos(pos);
}

void GLSCircle::render()
{
    GLSRenderObject::render();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    glLineWidth(.5);
    glColor3f(182/255., 231/255., 255/255.);
    
    glBegin(GL_LINES);
    for(int i = 0 ; i < 1024 ; i++)
    {
        float shift = 1280.f * 0.05f + static_cast<float>(i);
        glm::vec4 pos(shift, 360-radius_data_[i]*-1.5, 0, 1);
        glut_pos_to_gl_pos(&pos);
        glVertex2f(pos[0], pos[1]);
        glVertex2f(pos[0], -pos[1]);
    }
    glEnd();

#pragma region Debug
    /* DEBUG */
    
    glRasterPos2f(.8, avg_db_+.05);
    std::string dbstr = std::to_string(avg_db_ * 360 / 4);
    for(int idx = 0 ; idx < dbstr.length() ; idx++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, dbstr.at(idx));
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'd');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'B');

    glColor3f(1., .0, .0);
    glRasterPos2f(.0, -.5);
    std::string tstr = std::to_string(play_idx_);
    for(int idx = 0 ; idx < tstr.length() ; idx++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, tstr.at(idx));
    
    glColor3f(.7, .3, .2);
    glRasterPos2f(.0, -.6);
    tstr = std::to_string(static_cast<double>(play_time_) / 1'000.);
    for(int idx = 0 ; idx < tstr.length() ; idx++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, tstr.at(idx));
#pragma endregion
    
    glutSwapBuffers();
}

void GLSCircle::set_random_radius()
{
    for(int i = 0 ; i < CIRCULAR_PRECISION ; i++)
        radius_data_[i] = (static_cast<float>(rand()) / RAND_MAX + 1) * max_radius_;
}

void GLSCircle::set_radius(double* prev, double* radiuss, UINT32* window_size)
{
    for(UINT32 idx = 0 ; idx < *window_size ; idx++)
    {
        double gap = prev == nullptr ? 0 : abs(radiuss[idx] - prev[idx]);
        radius_data_[idx] = gap > 10 ? radiuss[idx] : 0; 
    }
}

void GLSCircle::play()
{
    LONGLONG term, length;
    UINT32 *shape = new UINT32[2];
    play_idx_ = 0;
    freq_info_ = music_reader_->output(&shape, &term, &length);
    radius_data_ = new double[shape[1]]{0.0};
    
    auto start_time = std::chrono::high_resolution_clock::now();
    auto until_time = std::chrono::high_resolution_clock::now();
    auto delay = std::chrono::nanoseconds(static_cast<UINT64>(length / shape[0]) * 100);
    do
    {
        draw_frame(freq_info_[play_idx_], &shape[1]);
        
        std::this_thread::sleep_until(until_time += delay);
        play_time_ = std::chrono::duration_cast<std::chrono::milliseconds>(until_time - start_time).count();
    }while(++play_idx_ < shape[0]);
}
