#include "GLSPhysics.h"

#include <gl/glew.h>
#include <glm/ext/matrix_clip_space.hpp>
#include "GLSCamera.h"
#include "../StageBuilder.h"

#define M_PI 3.141592653589793238462643383279502884197
#define M_E  2.718281828459045

typedef glm::mat4 mat4;

/*
 * GLSPhysics는 해당 Application에서 StageCurtain을 조작하는데에만 사용하므로,
 * StageCurtain이 위치한 Y좌표인 -2에 Fit하게 작동한다.
 */

vec3 GLSPhysics::screen_to_view(screen_pos position)
{
    GLSCamera* camera = StageBuilder::get_camera();

    GLint viewport_tmp[4];
    glGetIntegerv(GL_VIEWPORT, viewport_tmp);
    position[1] = viewport_tmp[3] - position[1] - 1;
    vec4  viewport(viewport_tmp[0], viewport_tmp[1], viewport_tmp[2], viewport_tmp[3]);
    
    mat4 projection = camera->get_perspective();
    mat4 view       = camera->get_lookAt();
    
    vec3 win_pos    = vec3(position[0], position[1], 0);
    vec3 near       = unProject(win_pos, mat4(1.0f) * view, projection, viewport);
    win_pos.z = 1.0f;
    vec3 far        = unProject(win_pos, mat4(1.0f) * view, projection, viewport);

    float t = (-2.0f - near.y) / (far.y - near.y);
    return near + t * (far - near);
}

float GLSPhysics::distance_from_point(vec3 point, float check_x)
{
    // 커튼을 지나가지 않으면 PASS
    if(std::min(initial_pos_.x, current_pos_.x) > check_x || std::max(initial_pos_.x, current_pos_.x) < check_x) return FLT_MAX;

    // Y = -2로 가정하고, X,Z 축 정보로 선분의 방정식 구하기 ( z = { (z2-z1)/(x2-x1) } * (x - x1) + z1 
    vec3   p1 = initial_pos_, p2 = current_pos_;
    float  z_value = (p2.z - p1.z) / (p2.x - p1.x) * (1.f - p1.x) + p1.z;
    return abs(z_value - point.z);
}

float GLSPhysics::get_power()
{
    bool   direct_to_minus = initial_pos_.x - current_pos_.x > 0;
    return length(current_pos_ - initial_pos_) * (direct_to_minus ? -1.f : 1.f);
}

float GLSPhysics::decay_norm_distribute(int range, float w)
{
    double t = 0.5; // 0.4일 때, range = 0에서 1
    /*
     *           1                      x^2
     *   -----------------  e ^ ( - ------------ )
     *   0.4 * sqrt(2 * π)           w * 0.4^2
     */
    double  distribute = 1.0 / ( t * sqrt(2 * M_PI) ) * pow(M_E, -pow(range, 2) / (w * pow(t, 2)));
    
    return static_cast<float>(distribute);
}

void GLSPhysics::start_physics(screen_pos initial)
{
    is_effecting_ = true;
    initial_pos_  = screen_to_view(initial);
    current_pos_  = initial_pos_;
}

void GLSPhysics::move_force(screen_pos position)
{
    if(!is_effecting_) return;
    current_pos_  = screen_to_view(position);
}

void GLSPhysics::end_physics()
{
    current_pos_  = {0, 0, 0};
    initial_pos_  = {0, 0, 0};
    is_effecting_ = false;
}
