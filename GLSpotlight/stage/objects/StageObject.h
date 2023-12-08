#pragma once
#include <windows.h>
#include <GL/gl.h>
#include <glm/glm.hpp>

typedef glm::vec3 vec3;
typedef glm::vec3* quad;
typedef glm::vec4 vec4;
typedef glm::vec4* affine;
typedef short size2d[2];
typedef GLfloat*   farr;

struct LightSource
{
    GLenum      id         ; // 광원 ID         
    farr        diffuse    ; // 난반사 광원 색상
    farr        specular   ; // 정반사 광원 색상
    farr        ambient    ; // 주변반사 광원 색상
    farr        direction  ; // 광원의 방향
    farr        attenuation; // Const, Linear, Quadratic
    bool        is_point   ; // 광원이 Point인지 Directional Light인지.
    float       cut_off    ; // 조명의 각도
    float       exponent   ; // Cosine 승수
};

struct Material
{
    farr    color    ; // Draw할 색상
    farr    ambient  ; // 주변 반사
    farr    diffuse  ; // 확산 반사
    farr    specular ; // 경면 반사
    farr    emission ; // 발광 색상
    float   shininess; // 광택 계수
};


class StageObject
{
    
protected:
    vec3 position_, rotate_, scale_;
    size2d window_size_;
    
    void update_window_size();
    void gl_pos_to_glut_pos(affine gl_pos);
    void glut_pos_to_gl_pos(affine glut_pos);
    void color_rgb(float r, float g, float b);
    void apply_material(GLenum face, Material mat);
    void apply_lightdata(LightSource light);

    GLfloat* get_rgba_by_ubyte(float red, float green, float blue, float alpha);

    virtual void pre_render();
    virtual void rendering() {  }
    virtual void post_render() { glPopMatrix(); }

    float get_random();
    quad make_quad(float x1, float y1, float z1, float x2, float y2, float z2);

public:
    virtual ~StageObject() = default;
    StageObject() : position_(0.f, 0.f, 0.f), rotate_(0.f, 0.f, 0.f), scale_(1.f, 1.f, 1.f), window_size_{0, 0} { }

    virtual void ready();
    
    void render()
    {
        pre_render();
        rendering();
        post_render();
    }

    void set_position(float x, float y, float z);
    void add_position(float x, float y, float z);
    void set_rotation(float x, float y, float z);
    
    static Material get_default_material();
    static LightSource get_default_light_source();
    
};
