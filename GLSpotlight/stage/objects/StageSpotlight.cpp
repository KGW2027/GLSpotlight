#include "StageSpotlight.h"

#include <thread>
#include <glm/common.hpp>

#include "../StageBuilder.h"
#include "../textures/TextureBase.h"

#define TEXTURE_CYL "Metal"
#define TEXTURE_LHT "Spotlight"

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

    load_texture(true, tid_metal);
    draw_cylinder(vec3(0, 0, 1), vec3(0, -90, 0), 0.3f, 1.5f);
    unload_texture();
    
    load_texture(false, tid_cam);
    draw_meshes(meshes_, 1);
    unload_texture();
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

StageSpotlight::StageSpotlight(GLenum light_source)
{
    light_data_ = get_default_light_source();
    light_data_.id = light_source;
    light_data_.diffuse  = get_rgba_by_ubyte(255, 255, 255, 1.0);
    light_data_.cut_off  = 40.0f;
    light_data_.exponent = 15.0f;
    
    rotate_[0] = 60;
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
    scale_ = {0.075, 0.075, 0.075};
    Material mat_camera = get_default_material();
    mat_camera.color   = get_rgba_by_ubyte(44, 44, 44, 1.0);
    mat_camera.ambient = new GLfloat[4]{0.3f, 0.3f, 0.3f, 1.0f};

    meshes_.push_back(Mesh{ // Downward
        make_quad(-1, -1, -1, 1, 1, -1), mat_camera
    });

    meshes_.push_back(Mesh{ // Upward
        make_quad(-1, -1, 1, 1, 1, 1), mat_camera
    });

    meshes_.push_back(Mesh{ // Right
        make_quad(-1, -1, -1, 1, -1, 1), mat_camera
    });
    
    meshes_.push_back(Mesh{ // Left
        make_quad(-1, 1, -1, 1, 1, 1), mat_camera
    });

    meshes_.push_back(Mesh{ // Back
        make_quad(-1, -1, -1, -1, 1, 1), mat_camera
    });
    
    meshes_.push_back(Mesh{ // Wing_Top
        make_quad(1, 1, -1, 1, -1, -2), mat_camera
    });
    
    meshes_.push_back(Mesh{ // Wing_Bottom
        make_quad(1, 1, 1, 1, -1, 2), mat_camera
    });
    
    meshes_.push_back(Mesh{ // Wing Right
        make_quad(1, -1, -1, 1, -2, 1), mat_camera
    });
    
    meshes_.push_back(Mesh{ // Wing Left
        make_quad(1, 1, -1, 1, 2, 1), mat_camera
    });

    tid_cam = TextureBase::get_texture(TEXTURE_LHT);
    tid_metal = TextureBase::get_texture(TEXTURE_CYL);
    
}

