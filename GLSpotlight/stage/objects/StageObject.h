#pragma once
#include <glm/glm.hpp>

typedef glm::vec3 vec3;
typedef glm::vec4* affine;
typedef short size2d[2];

class StageObject
{
    vec3 position_, rotate_, scale_;
    size2d window_size_;

    
    void update_window_size();
    void gl_pos_to_glut_pos(affine gl_pos);
    void glut_pos_to_gl_pos(affine glut_pos);
    
    virtual void pre_render() {}
    virtual void rendering() {}
    virtual void post_render() {}

public:
    virtual ~StageObject() = default;

    StageObject() : position_(0.f, 0.f, 0.f), rotate_(0.f, 0.f, 0.f), scale_(1.f, 1.f, 1.f), window_size_{0, 0} { }

    void render()
    {
        pre_render();
        rendering();
        post_render();
    }
    
};
