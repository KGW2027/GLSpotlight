#pragma once

#include <glm/glm.hpp>

typedef glm::vec4 screen_pos;
typedef int* window_size;

class GLSRenderObject
{
protected:
    window_size windowsize;
    
    void update_window_size();

    void gl_pos_to_glut_pos(glm::vec4* gl_pos);
    void glut_pos_to_gl_pos(glm::vec4* glut_pos);
    
public:
    GLSRenderObject() : windowsize(new int[2]) {}

    virtual void render();
    
};
