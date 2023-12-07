#include "StageRoom.h"


enum Expect_Axis
{
    EXPECT_X = 0x01,
    EXPECT_Y = 0x02,
    EXPECT_Z = 0x04
};


quad StageRoom::make_quad(float x1, float y1, float z1, float x2, float y2, float z2)
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

void StageRoom::pre_render()
{
    StageObject::pre_render();
}

void StageRoom::rendering()
{
    glBegin(GL_QUADS);
    for(RoomWall q : walls)
    {
        glMaterialfv(GL_FRONT, GL_AMBIENT, q.ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, q.diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, q.specular);
        glMateriali(GL_FRONT, GL_SHININESS, q.shiniess);
        color_rgb(q.colors[0], q.colors[1], q.colors[2]);
        for(unsigned int idx = 0 ; idx < 4 ; idx++)
            glVertex3f(q.points[idx][0], q.points[idx][1], q.points[idx][2]);
    }
    glEnd();
}

void StageRoom::post_render()
{
    StageObject::post_render();
}

void StageRoom::ready()
{
    StageObject::ready();

    float     *wall_color   = new float[3]{185, 122, 87};
    GLfloat *wall_ambient   = new GLfloat[4]{0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat *wall_diffuse   = new GLfloat[4]{0.7f, 0.7f, 0.7f, 1.f};
    GLfloat *wall_specular  = new GLfloat[4]{1.f, 1.f, 1.f, 1.f};
    int     wall_shininess  = 0.5;
    
    // Ceil & Floor
    walls.push_back(RoomWall{
        make_quad(-4, -2, -1, 4, 2, -1),
        wall_color, wall_ambient, wall_diffuse, wall_specular, wall_shininess
    });
    walls.push_back(RoomWall{
         make_quad(4, 2, 2, -4, -4, 2),
        wall_color, wall_ambient, wall_diffuse, wall_specular, wall_shininess
    });

    // Side Walls
    walls.push_back(RoomWall{
    make_quad(4, 2, 2, -4, 2, -1),
        wall_color, wall_ambient, wall_diffuse, wall_specular, wall_shininess
    });
    walls.push_back(RoomWall{
         make_quad(4, -4, 2, 4, 2, -1),
        wall_color, wall_ambient, wall_diffuse, wall_specular, wall_shininess
    });
    walls.push_back(RoomWall{
         make_quad(-4, -4, 2, -4, 2, -1),
        wall_color, wall_ambient, wall_diffuse, wall_specular, wall_shininess
    });
    walls.push_back(RoomWall{
         make_quad(-4, -4, -.5, 4, -4, 2),
        wall_color, wall_ambient, wall_diffuse, wall_specular, wall_shininess
    });

    float   *stage_color     = new float[3]{0, 122, 0};
    GLfloat *stage_ambient   = new GLfloat[4]{0.f, 0.f, 0.f, 0.f};
    GLfloat *stage_diffuse   = new GLfloat[4]{0.f, 0.f, 0.f, 0.f};
    GLfloat *stage_specular  = new GLfloat[4]{0.f, 0.f, 0.f, 0.f};
    int     stage_shininess  = 1;
    
    // Stage-Bottom Up
    walls.push_back(RoomWall{
         make_quad(-4, -2, -1, 4, -2, -.5),
        stage_color, stage_ambient, stage_diffuse, stage_specular, stage_shininess
    });
    walls.push_back(RoomWall{
         make_quad(-4, -2, -.5, 4, -4, -.5),
        stage_color, stage_ambient, stage_diffuse, stage_specular, stage_shininess
    });
}
