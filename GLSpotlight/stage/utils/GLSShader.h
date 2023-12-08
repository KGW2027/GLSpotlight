#pragma once

typedef unsigned int GLuint;

class GLSShader
{
    GLuint program_id_;
    
public:

    GLSShader();

    GLuint get_program_id() { return program_id_; }
};
