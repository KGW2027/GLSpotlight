#include "GLSCircle.h"

#include <math.h>
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

void GLSCircle::render()
{
    if(locked) return;
    locked = true;
    
    GLSRenderObject::render();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glBegin(GL_LINE_LOOP);
    glColor3f(0, 1, 0);
    for(int i = 0 ; i < CIRCULAR_PRECISION ; i++)
    {
        glm::vec4 pos;
        get_point_pos(static_cast<float>(i) * 360.f / CIRCULAR_PRECISION, radius_data[i], &pos);
        glVertex2f(pos[0], pos[1]);
    }
    glEnd();
    glutSwapBuffers();
    
    locked = false;
    
}

void GLSCircle::set_random_radius()
{
    if(locked) return;
    
    for(int i = 0 ; i < CIRCULAR_PRECISION ; i++)
        radius_data[i] = (static_cast<float>(rand()) / RAND_MAX + 1) * max_radius;
}
