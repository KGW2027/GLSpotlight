#include "StageRoom.h"

void StageRoom::pre_render()
{
    StageObject::pre_render();
}

void StageRoom::rendering()
{
    // Spotlight Hanger
    draw_cylinder(vec3(-1.77, -1.65, -3), vec3(0, 90, 0), 0.05f, 6);

    
    draw_meshes(walls, 20);
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
    walls.push_back(Mesh{ // Floor
        make_quad(-4, -2, -1, 4, 2, -1), mat_wall
    });
    walls.push_back(Mesh{ // Ceil
         make_quad(-4, 2, 2, 4, -4, 2), mat_wall
    });

    // Side Walls
    walls.push_back(Mesh{ // Entrance Wall (Camera Side)
    make_quad(4, 2, 2, -4, 2, -1), mat_wall
    });
    walls.push_back(Mesh{ // Left Wall
         make_quad(4, -4, 2, 4, 2, -1), mat_wall
    });
    walls.push_back(Mesh{ // Right Wall
         make_quad(-4, 2, 2, -4, -4, -1), mat_wall
    });
    walls.push_back(Mesh{ // Back-Stage Wall
         make_quad(4, -4, -.5, -4, -4, 2), mat_wall
    });
    
    // Stage-Bottom Up
    walls.push_back(Mesh{ // Stage Step
         make_quad(4, -2, -1, -4, -2, -.5), mat_stage
    });
    walls.push_back(Mesh{ // Stage Floor
         make_quad(4, -2, -.5, -4, -4, -.5), mat_stage
    });

}
