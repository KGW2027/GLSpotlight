
#include "StageObject.h"
#include <gl/freeglut.h>

void StageObject::update_window_size()
{
    window_size_[0] = static_cast<short>(glutGet(GLUT_WINDOW_WIDTH ));
    window_size_[1] = static_cast<short>(glutGet(GLUT_WINDOW_HEIGHT));
}

void StageObject::gl_pos_to_glut_pos(affine gl_pos)
{
    update_window_size();
    
    (*gl_pos)[0] = static_cast<float>(window_size_[0]) * ((*gl_pos)[0] + 1) / 2.f;

    float gp1 = (*gl_pos)[1];
    float height = window_size_[1];
    
    (*gl_pos)[1] = -(height * (gp1 + 1) / 2.f) + height;
}

void StageObject::glut_pos_to_gl_pos(affine glut_pos)
{
    update_window_size();
    (*glut_pos)[0] = (*glut_pos)[0] * 2 / static_cast<float>(window_size_[0]) - 1;
    (*glut_pos)[1] = (static_cast<float>(window_size_[1]) - (*glut_pos)[1]) * 2.f / static_cast<float>(window_size_[1]) - 1;
}

void StageObject::color_rgb(float r, float g, float b)
{
    glColor3f(r / 255.f, g / 255.f, b / 255.f);
}

void StageObject::pre_render()
{
    glPushMatrix();
    glTranslatef(position_[0], position_[1], position_[2]);
    glRotatef(rotate_[0], 1, 0, 0);
    glRotatef(rotate_[1], 0, 1, 0);
    glRotatef(rotate_[2], 0, 0, 1);
    glScalef(scale_[0], scale_[1], scale_[2]);
}
