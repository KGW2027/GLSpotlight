#pragma once
#include <glm/glm.hpp>

typedef glm::vec3  vec3;
typedef glm::ivec2 screen_pos;

class GLSPhysics
{
    vec3 initial_pos_ ;
    vec3 current_pos_ ;
    bool is_effecting_;

    vec3 screen_to_view(screen_pos position);

protected:
    
    bool  is_physics_enabled() { return is_effecting_; }
    float distance_from_point(vec3 point, float check_x);
    float get_power();

    static float decay_norm_distribute(int range, float w);
    
public:

    GLSPhysics() : initial_pos_(0, 0, 0), current_pos_(0, 0, 0), is_effecting_(false) {}

    void start_physics(screen_pos initial);
    void move_force(screen_pos position);
    void end_physics();
    
};
