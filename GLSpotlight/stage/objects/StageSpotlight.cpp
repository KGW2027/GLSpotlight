#include "StageSpotlight.h"

#include <thread>
#include <gl/freeglut.h>
#include <glm/gtc/type_ptr.hpp>

void StageSpotlight::pre_render()
{
    StageObject::pre_render();
}

void StageSpotlight::rendering()
{
    StageObject::rendering();

    if(abs(yaw_offset_) > 17.f)
    {
        yaw_offset_ = glm::clamp(yaw_offset_, -17.f, 17.f);
        yaw_direction_ *= -1;
    }

    yaw_offset_ += get_random() * yaw_direction_;
    rotate_[2] = 270 + yaw_offset_;
    
    calc_direction();
    draw_body();
}

void StageSpotlight::post_render()
{
    StageObject::post_render();
}

void StageSpotlight::calc_direction()
{
    glLightfv(light_data_.id, GL_POSITION, new GLfloat[4]{0, 0, 0, light_data_.is_point ? 1.f : 0.f});
    glLightfv(light_data_.id, GL_SPOT_DIRECTION, new GLfloat[3]{1, 0, 0});
}

void StageSpotlight::draw_body()
{
    
}

StageSpotlight::StageSpotlight(GLenum light_source)
{
    light_data_ = get_default_light_source();
    light_data_.id = light_source;
    light_data_.ambient  = get_rgba_by_ubyte(255, 255, 0, 1.0);
    light_data_.specular = get_rgba_by_ubyte(128, 128, 0, 1.0);
    light_data_.diffuse  = get_rgba_by_ubyte(255, 255, 255, 1.0);
    light_data_.cut_off  = 40.0f;
    light_data_.exponent = 15.0f;
    
    rotate_[0] = 60;
    rotate_[2] = 270;
}

void StageSpotlight::ready()
{
    StageObject::ready();

    // 조명 광원 세팅
    calc_direction();
    apply_lightdata(light_data_);

    yaw_offset_ = 0;
    yaw_direction_ = get_random() > 0.5f ? 1 : -1;

    // 조명 Body 세팅
    
    
}

