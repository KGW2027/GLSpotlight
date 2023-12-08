
#include "StageObject.h"
#include <algorithm>
#include <ctime>
#include <gl/freeglut.h>
#include <glm/common.hpp>

enum Expect_Axis
{
    EXPECT_X = 0x01,
    EXPECT_Y = 0x02,
    EXPECT_Z = 0x04
};


#pragma region Settings


float StageObject::get_random()
{
    return static_cast<float>(rand()) / RAND_MAX;
}

/**
 * \brief x,y,z에 대한 Rotator 정보를 이용해 Transformation Matrix를 회전시킨다.
 * \param rotator (x, y, z) - 각 축 회전에 대한 degree 정보
 */
void StageObject::rotate_self(vec3 rotator)
{
    glRotatef(rotator[0], 1, 0, 0);
    glRotatef(rotator[1], 0, 1, 0);
    glRotatef(rotator[2], 0, 0, 1);
}

void StageObject::load_texture(bool is_auto_mapping, unsigned texture_id)
{
    if(is_auto_mapping)
    {
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_id);
}

void StageObject::unload_texture()
{
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

/**
 * \brief Material 정보를 리셋합니다.
 */
void StageObject::reset_material()
{
    glColor3f(0, 0, 0);
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   zero_);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   zero_);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  zero_);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  zero_);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 0);
    
}

/**
 * \brief 0~255의 데이터를 받아 0~1 구간으로 변환 후 색으로 적용
 * \param r Red Color value (0-255)
 * \param g Green Color value (0-255)
 * \param b Blue Color value (0-255)
 */
void StageObject::color_rgb(float r, float g, float b)
{
    glColor3f(r / 255.f, g / 255.f, b / 255.f);
}

/**
 * \brief Material 구조체를 받아 Face에 적용
 * \param face 적용할 Face
 * \param mat  타겟 Material Struct
 */
void StageObject::apply_material(GLenum face, Material mat)
{
    if(mat.color[0] > 1.0 || mat.color[1] > 1.0 || mat.color[2] > 1.0)
        color_rgb(mat.color[0], mat.color[1], mat.color[2]);
    else
        glColor3f(mat.color[0], mat.color[1], mat.color[2]);
    
    glMaterialfv(face, GL_AMBIENT,   mat.ambient);
    glMaterialfv(face, GL_DIFFUSE,   mat.diffuse);
    glMaterialfv(face, GL_SPECULAR,  mat.specular);
    glMaterialfv(face, GL_EMISSION,  mat.emission);
    glMaterialf (face, GL_SHININESS, mat.shininess);
}

/**
 * \brief LightSource 구조체를 받아 빛 정보 반영
 * \param light 타겟 LightSource Struct
 */
void StageObject::apply_lightdata(LightSource light)
{
    glEnable(light.id);
    glLightfv(light.id, GL_AMBIENT,  light.ambient);
    glLightfv(light.id, GL_DIFFUSE,  light.diffuse);
    glLightfv(light.id, GL_SPECULAR, light.specular);
    glLightf (light.id, GL_SPOT_CUTOFF, light.cut_off);
    glLightf (light.id, GL_SPOT_EXPONENT, light.exponent);
    glLightf (light.id, GL_CONSTANT_ATTENUATION, light.attenuation[0]);
    glLightf (light.id, GL_LINEAR_ATTENUATION, light.attenuation[1]);
    glLightf (light.id, GL_QUADRATIC_ATTENUATION, light.attenuation[2]);
}

/**
 * \brief RGBA 정보를 0~255 값으로 받아 반환한다.
 * \param red  Red Color value (0-255)
 * \param green Green Color value (0-255)
 * \param blue  Blue Color value (0-255)
 * \param alpha Alpha Value (0-1)
 * \return 
 */
GLfloat* StageObject::get_rgba_by_ubyte(float red, float green, float blue, float alpha)
{
    red   = glm::clamp(red, 0.0f, 255.0f);
    blue  = glm::clamp(blue, 0.0f, 255.0f);
    green = glm::clamp(green, 0.0f, 255.0f);
    return new GLfloat[4]{red / 255.f, green / 255.f, blue / 255.f, alpha};
}


/**
 * \brief 점 2개의 3차원 좌표를 받아 Quad로 구성한다.
 * 두 정점의 x,y,z중 하나 이상이 같아야 한다.
 * x,y,z 중 하나가 같게 하기 위해서, 회전값을 먼저 수정해야할 수 있다.
 * \param x1 첫 번째 정점의 X 좌표
 * \param y1 첫 번째 정점의 Y 좌표
 * \param z1 첫 번째 정점의 Z 좌표
 * \param x2 두 번째 정점의 X 좌표
 * \param y2 두 번째 정점의 Y 좌표
 * \param z2 두 번째 정점의 Z 좌표
 * \return 두 정점을 대각선으로 갖는 면의 네 정점 좌표 정보 (vec3[4])
 */
quad StageObject::make_quad(float x1, float y1, float z1, float x2, float y2, float z2)
{
    quad q = new glm::vec3[4];
    Expect_Axis axis = x1 == x2 ? EXPECT_X : y1 == y2 ? EXPECT_Y : EXPECT_Z;
    switch(axis)
    {
    case EXPECT_X:
        q[0] = glm::vec3(x1, y1, z1);
        q[1] = glm::vec3(x1, y2, z1);
        q[2] = glm::vec3(x1, y2, z2);
        q[3] = glm::vec3(x1, y1, z2);
        break;
    case EXPECT_Y:
        q[0] = glm::vec3(x1, y1, z1);
        q[1] = glm::vec3(x2, y1, z1);
        q[2] = glm::vec3(x2, y1, z2);
        q[3] = glm::vec3(x1, y1, z2);
        break;
    case EXPECT_Z:
        q[0] = glm::vec3(x1, y1, z1);
        q[1] = glm::vec3(x2, y1, z1);
        q[2] = glm::vec3(x2, y2, z1);
        q[3] = glm::vec3(x1, y2, z1);
        break;
    }

    return q;
}

/**
 * \brief sx을 받아 dx, dy를 이용해 2차원 텍스처 Coordinate를 반환
 * \param sx 시작 x 지점
 * \param sy 시작 y 지점
 * \param dx +x 방향 변화값
 * \param dy +y 방향 변화값
 * \return 
 */
quad StageObject::make_coord(float sx, float sy, float dx, float dy)
{
    quad q = new glm::vec3[4];
    q[0] = glm::vec3(sx,        sy, 0);
    q[1] = glm::vec3(sx + dx, sy, 0);
    q[2] = glm::vec3(sx + dx, sy + dy, 0);
    q[3] = glm::vec3(sx, sy + dy, 0);
    return q;
}

#pragma endregion 

#pragma region Draw
/**
 * \brief Mesh 목록을 받아 월드에 렌더한다.<br/>Resolution에 20을 입력할 시, 20x20 으로 렌더된다.
 * \param meshes 렌더할 Mesh Vector
 * \param resolution_ 각 메시를 몇x몇 으로 분할하여 렌더할 것인지. 
 */
void StageObject::draw_meshes(std::vector<Mesh> meshes, const int resolution_)
{
    // 분할 해상도 (resolution_ 이 20일 경우, 면을 20x20으로 분할)
    float p1_res, p2_res;
    
    for(Mesh mesh : meshes)
    {
        // 최소좌표, 최대자표를 얻는다.
        float min_pos[3] = {9999, 9999, 9999}, max_pos[3] = {-9999, -9999, -9999};
        for(int pidx = 0 ; pidx < 4 ; pidx++)
        {
            for(int idx = 0 ; idx < 3 ; idx++)
            {
                min_pos[idx] = std::min(min_pos[idx], mesh.points[pidx][idx]);
                max_pos[idx] = std::max(max_pos[idx], mesh.points[pidx][idx]);
            }
        }

        // 최소좌표 최대좌표로 Mesh의 구성점과 구성면을 얻는다.
        std::vector<int> indices;
        int expect = -1;
        for(int idx = 0 ; idx < 3 ; idx++)
        {
            if(min_pos[idx] == max_pos[idx] && expect == -1)
                expect = idx;
            else 
                indices.push_back(idx);
        }

        p1_res  = std::max(0.05f, (max_pos[indices[0]] - min_pos[indices[0]]) / static_cast<float>(resolution_));
        p2_res  = std::max(0.05f, (max_pos[indices[1]] - min_pos[indices[1]]) / static_cast<float>(resolution_));

        // Material 적용
        apply_material(GL_FRONT_AND_BACK, mesh.material);
        
        // 입력된 해상도 값에 따라 Draw
        for(float p1 = min_pos[indices[0]] ; p1 < max_pos[indices[0]] ; p1 += p1_res)
        {
            for(float p2 = min_pos[indices[1]] ; p2 < max_pos[indices[1]] ; p2 += p2_res)
            {
                float margin = 0.00f;
                float s1 = p1 - margin, s2 = p2 - margin;
                float e1 = p1 + p1_res + margin, e2 = p2 + p2_res + margin;
                
                quad q = expect == 0 ? make_quad(min_pos[0], s1, s2, min_pos[0], e1, e2)
                       : expect == 1 ? make_quad(s1, min_pos[1], s2, e1, min_pos[1], e2)
                       : expect == 2 ? make_quad(s1, s2, min_pos[2], e1, e2, min_pos[2])
                       : nullptr;
                if(q == nullptr) continue;

                float loc_p1 = (max_pos[indices[0]] - p1) / (max_pos[indices[0]] - min_pos[indices[0]]);
                float loc_p2 = (max_pos[indices[1]] - p2) / (max_pos[indices[1]] - min_pos[indices[1]]);
                float delta  = 1.0f / static_cast<float>(resolution_);
                quad coord = make_coord(loc_p1, loc_p2, delta, delta);
                
                draw_quad_with_coord(q, coord);
            }
        }
    }
    
}

/**
 * \brief 원기둥을 그린다. Transformation Matrix는 변경되지 않는다.
 * \param origin 원기둥의 중앙점
 * \param rotate 원기둥의 회전 정보 (Rotator)
 * \param radius 원기둥의 반지름
 * \param height 원기둥의 높이
 */
void StageObject::draw_cylinder(vec3 origin, vec3 rotate, float radius, float height)
{
    glPushMatrix();
    rotate_self(rotate);
    glTranslatef(origin[0], origin[1], origin[2]);
    glutSolidCylinder(radius, height, 10, 6);
    glPopMatrix();
}


/**
 * \brief vec3[4] 을 이용해 QUAD를 그린다.
 * \param q vec3[4] - 면을 구성하는 점 4개의 정보
 */
void StageObject::draw_quad(quad q)
{
    glBegin(GL_QUADS);
    for(int idx = 0 ; idx < 4 ; idx++)
        glVertex3f(q[idx][0], q[idx][1], q[idx][2]);
    glEnd();
}

/**
 * \brief vec3[4]와 vec2[4]를 이용해 QUAD를 그린다.
 * \param q vec3[4] - 면을 구성하는 점 4개의 정보
 * \param coord vec2[4] - 각 점의 coordinate
 */
void StageObject::draw_quad_with_coord(quad q, quad coord)
{
    glBegin(GL_QUADS);
    for(int idx = 0 ; idx < 4 ; idx++)
    {
        glTexCoord2f(coord[idx][0], coord[idx][1]);
        glVertex3f(q[idx][0], q[idx][1], q[idx][2]);
    }
    glEnd();
}

#pragma endregion

#pragma region Virtual

/**
 * \brief rendering() 함수 실행 이전에 실행되는 함수.<br/>transformation matrix의 변환이 이뤄진다.
 */
void StageObject::pre_render()
{
    glPushMatrix();
    glTranslatef(position_[0], position_[1], position_[2]);
    rotate_self(rotate_);
    glScalef(scale_[0], scale_[1], scale_[2]);
}

/**
 * \brief 오브젝트를 등록할 때 단 한 번 실행된다.
 */
void StageObject::ready()
{
    srand(time(0));
}

#pragma endregion 

/**
 * \brief 오브젝트의 월드 좌표를 설정한다.
 */
void StageObject::set_position(float x, float y, float z)
{
    position_[0] = x;
    position_[1] = y;
    position_[2] = z;
}

/**
 * \brief 오브젝트의 상대 좌표를 설정한다.
 */
void StageObject::add_position(float x, float y, float z)
{
    position_[0] += x;
    position_[1] += y;
    position_[2] += z;
}

/**
 * \brief 오브젝트의 로테이터를 설정한다.
 */
void StageObject::set_rotation(float x, float y, float z)
{
    rotate_[0] = x;
    rotate_[1] = y;
    rotate_[2] = z;
}

/**
 * \brief 기본 Material Struct를 반환한다.
 */
Material StageObject::get_default_material()
{
    return Material {
        new GLfloat[3] {0.f, 0.f, 0.f},
        new GLfloat[4] {0.2f, 0.2f, 0.2f, 1.0f},
         new GLfloat[4] {0.8f, 0.8f, 0.8f, 1.0f},
        new GLfloat[4] {0.0f, 0.0f, 0.0f, 1.0f},
        new GLfloat[4] {0.0f, 0.0f, 0.0f, 0.0f},
        0.0
    };
}

/**
 * \brief 기본 LightSource Struct를 반환한다.
 */
LightSource StageObject::get_default_light_source()
{
    return LightSource {
        0,
        new GLfloat[4]{0.0f, 0.0f, 0.0f, 1.0f},
        new GLfloat[4]{0.0f, 0.0f, 0.0f, 1.0f},
        new GLfloat[4]{0.0f, 0.0f, 0.0f, 0.0f},
        new GLfloat[3]{1.0f, 0.0f, 0.0f},
        new GLfloat[3]{1.0f, 0.2f, 0.10f},
        true,
        90.0f,
        0.0f
    };
}
