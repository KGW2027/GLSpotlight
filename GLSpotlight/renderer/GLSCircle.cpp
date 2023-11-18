#include "GLSCircle.h"

#include <math.h>
#include <windows.h>
#include <stdio.h>
#include <GL/gl.h>


void GLSCircle::get_point_pos(float theta, float radius, screen_pos* pos)
{
    screen_pos center = new float[3]{circle_center[0], circle_center[1], circle_center[2]};
    
    gl_pos_to_glut_pos(&center);
    float radian = theta * (3.1415f / 180.f);
    (*pos)[0] = center[0] + radius * static_cast<float>(cos(radian));
    (*pos)[1] = center[1] + radius * static_cast<float>(sin(radian));
    glut_pos_to_gl_pos(pos);
}

void GLSCircle::render()
{
    GLSRenderObject::render();
    set_random_radius();
    
    glColor3f(1, 1, 1);
    glBegin(GL_LINE_LOOP);
    screen_pos pos = new float[3]{0};
    for(int i = 0 ; i < CIRCULAR_PRECISION ; i++)
    {
        get_point_pos(static_cast<float>(i) * 360.f / CIRCULAR_PRECISION, max(min_radius, radius_data[i]), &pos);
        glVertex3f(pos[0], pos[1], pos[2]);
    }
    glEnd();
    glFlush();
    
}
