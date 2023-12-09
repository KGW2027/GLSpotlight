#include "StageRoom.h"

#include "../textures/TextureBase.h"

#define TEXTURE_CYL "Metal"
#define TEXTURE_ROOM "Room"

void StageRoom::pre_render()
{
    StageObject::pre_render();
}

void StageRoom::rendering()
{
    // Spotlight Hanger
    
    load_texture(true, tid_metal);
    draw_cylinder(vec3(-1.77, -1.65, -3), vec3(0, 90, 0), 0.05f, 8);
    unload_texture();

    // Draw Room
    load_texture(false, tid_room);
    draw_meshes(meshes_, 20);
    unload_texture();
}

void StageRoom::post_render()
{
    StageObject::post_render();
}

void StageRoom::ready()
{
    StageObject::ready();

    Material mat_wall = get_default_material();
    mat_wall.color   = get_rgba_by_ubyte(25, 0, 0, 1.0);
    mat_wall.ambient = new GLfloat[4]{0.f, 0.f, 0.f, 1.0f};
    
    // Ceil & Floor
    meshes_.push_back(Mesh{ // Floor
        make_quad(-4, -2, -1, 4, 2, -1), mat_wall
    });
    meshes_.push_back(Mesh{ // Ceil
         make_quad(-4, 2, 2, 4, -4, 2), mat_wall
    });

    // Side Walls
    meshes_.push_back(Mesh{ // Entrance Wall (Camera Side)
    make_quad(4, 2, 2, -4, 2, -1), mat_wall
    });
    meshes_.push_back(Mesh{ // Left Wall
         make_quad(4, -4, 2, 4, 2, -1), mat_wall
    });
    meshes_.push_back(Mesh{ // Right Wall
         make_quad(-4, 2, 2, -4, -4, -1), mat_wall
    });
    meshes_.push_back(Mesh{ // Back-Stage Wall
         make_quad(4, -4, -.5, -4, -4, 2), mat_wall
    });

    tid_room = TextureBase::get_texture(TEXTURE_ROOM);
    tid_metal = TextureBase::get_texture(TEXTURE_CYL);

}
