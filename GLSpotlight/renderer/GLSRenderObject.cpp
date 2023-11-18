#include "GLSRenderObject.h"

#include <gl/freeglut.h>
#include <GL/gl.h>

void GLSRenderObject::render()
{
    glClear(GL_COLOR_BUFFER_BIT);
}


void GLSRenderObject::update_window_size()
{
    windowsize[0] = glutGet(GLUT_WINDOW_WIDTH);
    windowsize[1] = glutGet(GLUT_WINDOW_HEIGHT);
}

void GLSRenderObject::gl_pos_to_glut_pos(float** gl_pos)
{
    update_window_size();
    
    (*gl_pos)[0] = static_cast<float>(windowsize[0]) * ((*gl_pos)[0] + 1) / 2.f;

    float gp1 = (*gl_pos)[1];
    float height = static_cast<float>(windowsize[1]);
    
    (*gl_pos)[1] = -(height * (gp1 + 1) / 2.f) + height;
}

void GLSRenderObject::glut_pos_to_gl_pos(float** glut_pos)
{
    update_window_size();
    (*glut_pos)[0] = (*glut_pos)[0] * 2 / static_cast<float>(windowsize[0]) - 1;
    (*glut_pos)[1] = (static_cast<float>(windowsize[1]) - (*glut_pos)[1]) * 2.f / static_cast<float>(windowsize[1]) - 1;
}  
