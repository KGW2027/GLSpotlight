
#include "StageObject.h"

#include <algorithm>
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

void StageObject::apply_material(GLenum face, Material mat)
{
    if(mat.color[0] > 1.0 || mat.color[1] > 1.0 || mat.color[2] > 1.0)
        color_rgb(mat.color[0], mat.color[1], mat.color[2]);
    else
        glColor3f(mat.color[0], mat.color[1], mat.color[2]);
    
    glMaterialfv(face, GL_AMBIENT,   mat.ambient);
    glMaterialfv(face, GL_DIFFUSE,   mat.diffuse);
    glMaterialfv(face, GL_SPECULAR,  mat.specular);
    glMaterialfv(face, GL_EMISSION,  mat.emission);
    glMaterialf (face, GL_SHININESS, mat.shininess);
}

void StageObject::apply_lightdata(LightSource light)
{
    glEnable(light.id);
    glLightfv(light.id, GL_AMBIENT,  light.ambient);
    glLightfv(light.id, GL_DIFFUSE,  light.diffuse);
    glLightfv(light.id, GL_SPECULAR, light.specular);
    glLightf (light.id, GL_SPOT_CUTOFF, light.cut_off);
    glLightf (light.id, GL_SPOT_EXPONENT, light.exponent);
    glLightf (light.id, GL_CONSTANT_ATTENUATION, light.attenuation[0]);
    glLightf (light.id, GL_LINEAR_ATTENUATION, light.attenuation[1]);
    glLightf (light.id, GL_QUADRATIC_ATTENUATION, light.attenuation[2]);
}

GLfloat* StageObject::get_rgba_by_ubyte(float red, float green, float blue, float alpha)
{
    red   = glm::clamp(red, 0.0f, 255.0f);
    blue  = glm::clamp(blue, 0.0f, 255.0f);
    green = glm::clamp(green, 0.0f, 255.0f);
    return new GLfloat[4]{red / 255.f, green / 255.f, blue / 255.f, alpha};
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

Material StageObject::get_default_material()
{
    return Material {
        new GLfloat[3] {0.f, 0.f, 0.f},
        new GLfloat[4] {0.2f, 0.2f, 0.2f, 1.0f},
         new GLfloat[4] {0.8f, 0.8f, 0.8f, 1.0f},
        new GLfloat[4] {0.0f, 0.0f, 0.0f, 1.0f},
        new GLfloat[4] {0.0f, 0.0f, 0.0f, 0.0f},
        0.0
    };
}

LightSource StageObject::get_default_light_source()
{
    return LightSource {
        0,
        new GLfloat[4]{0.0f, 0.0f, 0.0f, 1.0f},
        new GLfloat[4]{0.0f, 0.0f, 0.0f, 1.0f},
        new GLfloat[4]{0.0f, 0.0f, 0.0f, 0.0f},
        new GLfloat[3]{1.0f, 0.0f, 0.0f},
        new GLfloat[3]{1.0f, 0.2f, 0.10f},
        true,
        90.0f,
        0.0f
    };
}
