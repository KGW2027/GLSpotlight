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
        apply_material(GL_FRONT, q.material);
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

    Material mat_wall = get_default_material();
    mat_wall.color   = get_rgba_by_ubyte(255, 0, 0, 1.0);
    mat_wall.ambient = new GLfloat[4]{0.3f, 0.3f, 0.3f, 1.0f};

    Material mat_stage = get_default_material();
    mat_stage.color   = new GLfloat[3]{0, 122, 0};
    mat_stage.ambient = new GLfloat[4]{0.5f, 0.5f, 0.5f, 1.0f}; 
    
    // Ceil & Floor
    walls.push_back(RoomWall{ // Floor
        make_quad(-4, -2, -1, 4, 2, -1), mat_wall
    });
    walls.push_back(RoomWall{ // Ceil
         make_quad(-4, 2, 2, 4, -4, 2), mat_wall
    });

    // Side Walls
    walls.push_back(RoomWall{ // Entrance Wall (Camera Side)
    make_quad(4, 2, 2, -4, 2, -1), mat_wall
    });
    walls.push_back(RoomWall{ // Left Wall
         make_quad(4, -4, 2, 4, 2, -1), mat_wall
    });
    walls.push_back(RoomWall{ // Right Wall
         make_quad(-4, 2, 2, -4, -4, -1), mat_wall
    });
    walls.push_back(RoomWall{ // Back-Stage Wall
         make_quad(4, -4, -.5, -4, -4, 2), mat_wall
    });
    
    // Stage-Bottom Up
    walls.push_back(RoomWall{ // Stage Step
         make_quad(4, -2, -1, -4, -2, -.5), mat_stage
    });
    walls.push_back(RoomWall{ // Stage Floor
         make_quad(4, -2, -.5, -4, -4, -.5), mat_stage
    });
}
