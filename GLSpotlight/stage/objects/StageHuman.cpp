#include "StageHuman.h"

#include <gl/freeglut.h>
#include <glm/common.hpp>
#include <glm/detail/func_trigonometric.inl>

#define M_PI 3.141592653589793238462643383279502884197

void StageHuman::move_arm()
{
    interp_point_ += (arm_ascend_ ? -1.f : 1.f) * (arm_mode_ == FWD ? 0.03f : 0.01f);
    if( (arm_ascend_ && interp_point_ <= 0.0f) || (!arm_ascend_ && interp_point_ >= 1.0f) )
        arm_ascend_ = !arm_ascend_;
    interp_point_ = glm::clamp(interp_point_, 0.0f, 1.0f);

    arm_deg_ =  90.f * static_cast<float>(sin(M_PI * (interp_point_ - 0.5f)) + 1.f) / 2.f;
    arm_deg_ -= 45.f;
}

GLfloat* StageHuman::random_color(int i)
{
    switch(i)
    {
    case 0:
        return get_rgba_by_ubyte(255, 0, 0, 1.0f);
    case 1:
        return get_rgba_by_ubyte(255, 50, 0, 1.0f);
    case 2:
        return get_rgba_by_ubyte(255, 255, 0, 1.0f);
    case 3:
        return get_rgba_by_ubyte(0, 255, 0, 1.0f);
    case 4:
        return get_rgba_by_ubyte(0, 0, 255, 1.0f);
    case 5:
        return get_rgba_by_ubyte(0, 5, 255, 1.0f);
    case 6:
        return get_rgba_by_ubyte(100, 0, 255, 1.0f);
    }
    return get_rgba_by_ubyte(0, 0, 0, 1.0f);
}

void StageHuman::pre_render()
{
    // if(++color_change_ >= 100)
    // {
    //     color_change_ = 0;
    //     GLfloat* new_color = random_color(static_cast<int>(floor(get_random() * 8.f)));
    //     target_color_ = {new_color[0], new_color[1], new_color[2]};
    //     free(new_color);
    // }
    //
    // for(int i = 0 ; i < 3 ; i++)
    //     cur_color_[i] -= (cur_color_[i] - target_color_[i]) * 0.005f;
    
    StageObject::pre_render();
    move_arm();
}

void StageHuman::rendering()
{
    StageObject::rendering();


    if(arm_mode_ == FWD)
        rotate_self({arm_deg_, 0, 0});
    else if(arm_mode_ == SIDE)
        rotate_self({0, arm_deg_, 0});
    
    apply_material(GL_FRONT_AND_BACK, mat_head_);
    glutSolidSphere(1, 10, 4);

    for(int i = 0 ; i < 3 ; i++) mat_stick_.color[i] = cur_color_[i];
    apply_material(GL_FRONT_AND_BACK, mat_stick_);

    glPushMatrix();
    glTranslatef(1.3f, 0.0f, 0.0f);
    glutSolidCylinder(0.1f, 2.f, 10, 4);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1.3f, 0.0f, 0.0f);
    glutSolidCylinder(0.1f, 2.f, 10, 4);
    glPopMatrix();
}

void StageHuman::post_render()
{
    StageObject::post_render();
}

StageHuman::StageHuman(float x, float y)
{
    position_[0] = x;
    position_[1] = y;
    position_[2] = -.75;
    scale_ = {0.1, 0.1, 0.1};
}

void StageHuman::ready()
{
    StageObject::ready();

    interp_point_ = 0.0f;
    arm_deg_    = 0;
    arm_ascend_ = false;
    arm_mode_   = FWD;

    color_change_ = 0;
    cur_color_ = {1.0f, 0.f, 0.f};
    target_color_ = {1.0f, 0.f, 0.f};

    mat_head_ = get_default_material();
    mat_head_.color = get_rgba_by_ubyte(25, 25, 25, 1.0f);

    mat_stick_ = get_default_material();
}
