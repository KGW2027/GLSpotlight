
#include "StageObject.h"

#include <algorithm>
#include <ctime>
#include <gl/freeglut.h>

enum Expect_Axis
{
    EXPECT_X = 0x01,
    EXPECT_Y = 0x02,
    EXPECT_Z = 0x04
};

void StageObject::draw_quad(quad q)
{
    glBegin(GL_QUADS);
    for(int idx = 0 ; idx < 4 ; idx++)
        glVertex3f(q[idx][0], q[idx][1], q[idx][2]);
    glEnd();
}

void StageObject::rotate_self(vec3 rotator)
{
    glRotatef(rotator[0], 1, 0, 0);
    glRotatef(rotator[1], 0, 1, 0);
    glRotatef(rotator[2], 0, 0, 1);
}

void StageObject::update_window_size()
{
    window_size_[0] = static_cast<short>(glutGet(GLUT_WINDOW_WIDTH ));
    window_size_[1] = static_cast<short>(glutGet(GLUT_WINDOW_HEIGHT));
}

void StageObject::gl_pos_to_glut_pos(affine gl_pos)
{
    update_window_size();
    
    (*gl_pos)[0] = static_cast<float>(window_size_[0]) * ((*gl_pos)[0] + 1) / 2.f;

    float gp1 = (*gl_pos)[1];
    float height = window_size_[1];
    
    (*gl_pos)[1] = -(height * (gp1 + 1) / 2.f) + height;
}

void StageObject::glut_pos_to_gl_pos(affine glut_pos)
{
    update_window_size();
    (*glut_pos)[0] = (*glut_pos)[0] * 2 / static_cast<float>(window_size_[0]) - 1;
    (*glut_pos)[1] = (static_cast<float>(window_size_[1]) - (*glut_pos)[1]) * 2.f / static_cast<float>(window_size_[1]) - 1;
}

void StageObject::color_rgb(float r, float g, float b)
{
    glColor3f(r / 255.f, g / 255.f, b / 255.f);
}

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
                
                draw_quad(q);
            }
        }
    }
    
}

void StageObject::draw_cylinder(vec3 origin, vec3 rotate, float radius, float height)
{
    glPushMatrix();
    rotate_self(rotate);
    glTranslatef(origin[0], origin[1], origin[2]);
    glutSolidCylinder(radius, height, 10, 6);
    glPopMatrix();
}

GLfloat* StageObject::get_rgba_by_ubyte(float red, float green, float blue, float alpha)
{
    red   = glm::clamp(red, 0.0f, 255.0f);
    blue  = glm::clamp(blue, 0.0f, 255.0f);
    green = glm::clamp(green, 0.0f, 255.0f);
    return new GLfloat[4]{red / 255.f, green / 255.f, blue / 255.f, alpha};
}

void StageObject::pre_render()
{
    glPushMatrix();
    glTranslatef(position_[0], position_[1], position_[2]);
    rotate_self(rotate_);
    glScalef(scale_[0], scale_[1], scale_[2]);
}

float StageObject::get_random()
{
    return static_cast<float>(rand()) / RAND_MAX;
}

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

void StageObject::ready()
{
    srand(time(0));
}

void StageObject::set_position(float x, float y, float z)
{
    position_[0] = x;
    position_[1] = y;
    position_[2] = z;
}

void StageObject::add_position(float x, float y, float z)
{
    position_[0] += x;
    position_[1] += y;
    position_[2] += z;
}

void StageObject::set_rotation(float x, float y, float z)
{
    rotate_[0] = x;
    rotate_[1] = y;
    rotate_[2] = z;
}

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
