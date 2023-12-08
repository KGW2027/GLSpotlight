#pragma once
#include <gl/glew.h>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

typedef glm::vec3 vec3;
typedef glm::vec3* quad;
typedef glm::vec4 vec4;
typedef glm::vec4* affine;
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

struct Mesh
{
    quad        points   ; // Mesh를 구성하는 vertex 집합
    Material    material ; // Mesh의 머테리얼
};

class StageObject
{
    const GLfloat *zero_ = new GLfloat[4]{.0f, .0f, .0f, 1.f}; 
    
protected:
    vec3 position_, rotate_, scale_;
    std::vector<Mesh> meshes_;

#pragma region Settings
    void color_rgb(float r, float g, float b);
    void apply_material(GLenum face, Material mat);
    void apply_lightdata(LightSource light);
    void rotate_self(vec3 rotator);
    void load_texture(bool is_auto_mapping, unsigned int texture_id);
    void unload_texture();
    void reset_material();
    float get_random();
    quad make_quad(float x1, float y1, float z1, float x2, float y2, float z2);
    quad make_coord(float sx, float sy, float dx, float dy);
    GLfloat* get_rgba_by_ubyte(float red, float green, float blue, float alpha);
#pragma endregion

#pragma region Draw
    void draw_quad(quad q);
    void draw_quad_with_coord(quad q, quad coord);
    void draw_meshes(std::vector<Mesh> meshes, int resolution_);
    void draw_cylinder(vec3 origin, vec3 rotate, float radius, float height);
#pragma endregion 

#pragma region Virtual
    virtual void pre_render();
    virtual void rendering() {  }
    virtual void post_render() { glPopMatrix(); reset_material(); }
#pragma endregion 

public:
    virtual ~StageObject() = default;
    StageObject() : position_(0.f, 0.f, 0.f), rotate_(0.f, 0.f, 0.f), scale_(1.f, 1.f, 1.f) { }

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
