#include "StageSpotlight.h"

#include <gl/freeglut.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void StageSpotlight::pre_render()
{
    rotate_[2] += 1.0f;
    if(rotate_[2] >= 360.f) rotate_[2] = 0.0f;
    printf("(%f, %f, %f)\n", rotate_[0], rotate_[1], rotate_[2]);
    StageObject::pre_render();
}

void StageSpotlight::rendering()
{
    StageObject::rendering();

    calc_direction();
    
    color_rgb(0, 0, 200);
    glutWireSphere(0.05, 4, 2);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);
    glEnd();
}

void StageSpotlight::post_render()
{
    StageObject::post_render();
}

void StageSpotlight::calc_direction()
{
    GLfloat buffer[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, buffer);
    glm::mat4 matrix = glm::make_mat4(buffer);
    vec3 forward = matrix * vec4(1, 0, 0, 1);
    glLightfv(light_data_.id, GL_POSITION, new GLfloat[4]{0, 0, 0, light_data_.is_point ? 1.f : 0.f});
    glLightfv(light_data_.id, GL_SPOT_DIRECTION, new GLfloat[3]{forward[0], forward[1], forward[2]});
}

StageSpotlight::StageSpotlight(GLenum light_source)
{
    light_data_ = get_default_light_source();
    light_data_.id = light_source;
    light_data_.ambient  = get_rgba_by_ubyte(128, 0, 0, 1.0);
    light_data_.diffuse  = get_rgba_by_ubyte(255, 128, 128, 1.0);
    light_data_.specular = get_rgba_by_ubyte(255, 255, 255, 1.0);

    position_[2] += 1;
}

void StageSpotlight::ready()
{
    StageObject::ready();
    calc_direction();
    apply_lightdata(light_data_);
    
}
