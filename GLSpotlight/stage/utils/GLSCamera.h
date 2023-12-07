#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

typedef glm::ivec2 ivec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::mat4 mat4;

class GLSCamera
{
    
    vec3    eye_;
    vec3    at_;
    vec3    up_;
    vec3    forward_;
    vec3    right_;
    
    float   arm;

    ivec2   move_start_;
    ivec2   move_end_  ;

    void rotate_camera(float delta, bool is_x);

    void update_state()
    {
        up_ = normalize(up_);
        arm = length(at_ - eye_);
        forward_ = normalize(at_ - eye_);
        right_ = normalize(cross(forward_, up_));
    }
    
public:

    GLSCamera()
    {
        eye_        = {2.72, 1.8, 1.57};
        at_         = {0, -.3, 0.5};
        up_         = {0, 0, 1};
        move_start_ = {0, 0};
        move_end_   = {0, 0};
        arm           = .3f;

        update_state();
    }

    void update();

    void mouse_click(int button, int state, int x, int y);
    void mouse_move(int x, int y);
    void mouse_wheel(int unk, int direction, int x, int y);
    
};
