#include "GLSCircle.h"

#include <windows.h>
#include <stdio.h>
#include <gl/freeglut.h>
#include <GL/gl.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


void GLSCircle::get_point_pos(float theta, float radius, screen_pos* pos)
{
    glm::vec4 dir_vec = glm::vec4(radius, 0, 0, 1);
    glm::mat4 rot = rotate(glm::mat4(1), glm::radians(theta), glm::vec3(0, 0, 1));
    *pos = circle_center + dir_vec * rot;
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
    for(int i = 0 ; i < CIRCULAR_PRECISION ; i++)
    {
        set_hue_based_color(i);
        if(radius_data[i] < max_radius/4) continue;
        glm::vec4 center, pos;
        get_point_pos(static_cast<float>(i) * 360.f / CIRCULAR_PRECISION, max_radius/4, &center);
        get_point_pos(static_cast<float>(i) * 360.f / CIRCULAR_PRECISION, radius_data[i], &pos);
        glVertex2f(center[0], center[1]);
        glVertex2f(pos[0], pos[1]);
    }
    glEnd();
    glutSwapBuffers();
}

void GLSCircle::set_random_radius()
{
    if(locked) return;
    
    for(int i = 0 ; i < CIRCULAR_PRECISION ; i++)
        radius_data[i] = (static_cast<float>(rand()) / RAND_MAX + 1) * max_radius;
}

void GLSCircle::set_radius(float* radiuss)
{
    for(int i = 0 ; i < CIRCULAR_PRECISION ; i++)
        radius_data[i] = max_radius * radiuss[i];
}
