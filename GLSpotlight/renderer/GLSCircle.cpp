#include "GLSCircle.h"

#include <windows.h>
#include <stdio.h>
#include <string>
#include <thread>
#include <gl/freeglut.h>
#include <GL/gl.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


void GLSCircle::get_point_pos(float theta, float radius, screen_pos* pos)
{
    glm::vec4 dir_vec = glm::vec4(radius, 0, 0, 1);
    glm::mat4 rot = rotate(glm::mat4(1), glm::radians(theta), glm::vec3(0, 0, 1));
    *pos = circle_center_ + dir_vec * rot;
    glut_pos_to_gl_pos(pos);
}

void GLSCircle::set_hue_based_color(int idx)
{
    int range;
    float s, v, c, x, m, pr, pg, pb;
    range = idx * (CIRCULAR_PRECISION/360.f) / 60.f;
    s = v = 1.f; s -=.5f;
    c = s*v;
    x = c * (1.f - abs(range % 2 - 1));
    m = v - c;

    pr = range == 0 || range == 5 ? c : range == 1 || range == 4 ? x : 0;
    pg = range == 0 || range == 3 ? x : range == 1 || range == 2 ? c : 0;
    pb = range == 2 || range == 5 ? x : range == 3 || range == 4 ? c : 0;
    glColor3f(pr+m, pg+m, pb+m);
}

void GLSCircle::render()
{
    GLSRenderObject::render();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glBegin(GL_LINES);
    glLineWidth(.1f);

    // Line Render
    // TODO - dB를 전체 평균내서 정규화한다음 가로로 길게 그려서 볼륨체크.
    for(int i = 0 ; i < CIRCULAR_PRECISION ; i++)
    {
        set_hue_based_color(i);
        float shift = 1280.f * 0.9f / CIRCULAR_PRECISION * static_cast<float>(i+1) + 1280.f * 0.05;
        glm::vec4 pos(shift, 360-radius_data_[i], 0, 1), btm(shift, 360, 0, 1);
        glut_pos_to_gl_pos(&pos);
        glut_pos_to_gl_pos(&btm);
        glVertex2f(pos[0], pos[1]);
        glVertex2f(btm[0], btm[1]);
    }
    
    // Circular Render
    // for(int i = 0 ; i < CIRCULAR_PRECISION ; i++)
    // {
    //     set_hue_based_color(i);
    //     if(radius_data[i] < max_radius/4) continue;
    //     glm::vec4 center, pos;
    //     get_point_pos(static_cast<float>(i) * 360.f / CIRCULAR_PRECISION, max_radius/4, &center);
    //     get_point_pos(static_cast<float>(i) * 360.f / CIRCULAR_PRECISION, radius_data[i], &pos);
    //     glVertex2f(center[0], center[1]);
    //     glVertex2f(pos[0], pos[1]);
    // }
    glEnd();
    
    glBegin(GL_LINES);
    glColor3f(1, 1, 1);
    glVertex2f(-.9, avg_db_);
    glVertex2f(.9, avg_db_);
    glEnd();

    glRasterPos2f(.8, avg_db_+.05);
    std::string dbstr = std::to_string(avg_db_ * 360 / 4);
    for(int idx = 0 ; idx < dbstr.length() ; idx++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, dbstr.at(idx));
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'd');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'B');

    glColor3f(1.f, .0, .0);
    glRasterPos2f(.0, -.5);
    std::string tstr = std::to_string(play_idx_);
    for(int idx = 0 ; idx < tstr.length() ; idx++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, tstr.at(idx));
    
    glutSwapBuffers();
}

void GLSCircle::set_random_radius()
{
    for(int i = 0 ; i < CIRCULAR_PRECISION ; i++)
        radius_data_[i] = (static_cast<float>(rand()) / RAND_MAX + 1) * max_radius_;
}

void GLSCircle::set_radius(double* radiuss, UINT32* window_size)
{
    double* weight = new double[CIRCULAR_PRECISION]{.0f};
    double dmax = -1, sum = 0;
    float range = static_cast<float>(*window_size) / static_cast<float>(CIRCULAR_PRECISION);

    for(UINT32 idx = 0 ; idx < *window_size ; idx++)
    {
        dmax = max(dmax, radiuss[idx]);
        sum += radiuss[idx];
        weight[static_cast<int>(idx / range)] += radiuss[idx] / range;
    }

    avg_db_ = sum / *window_size * 4 / 360.0;
    
    for(int i = 0 ; i < CIRCULAR_PRECISION ; i++)
        radius_data_[i] = weight[i];
}

void GLSCircle::play()
{
    LONGLONG term, length;
    UINT32 *shape = new UINT32[2];
    play_idx_ = 0;
    freq_info_ = music_reader_->output(&shape, &term, &length);
    printf("Shape (%d, %d)\n", shape[0], shape[1]);
    do
    {
        set_radius(freq_info_[play_idx_], &shape[1]);
        std::this_thread::sleep_for(std::chrono::nanoseconds(length * 100 / shape[0]));
    }while(++play_idx_ < shape[0]);
}
