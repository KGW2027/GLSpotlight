#include "StageRoom.h"


void StageRoom::draw_high_resolution(quad points)
{
    float min_pos[3] = {9999, 9999, 9999}, max_pos[3] = {-9999, -9999, -9999};
    std::vector<vec3> debug_vec3s;
    for(int pidx = 0 ; pidx < 4 ; pidx++)
    {
        debug_vec3s.push_back(points[pidx]);
        for(int idx = 0 ; idx < 3 ; idx++)
        {
            min_pos[idx] = std::min(min_pos[idx], points[pidx][idx]);
            max_pos[idx] = std::max(max_pos[idx], points[pidx][idx]);
        }
    }

    std::vector<int> indices;
    int expect = -1;
    for(int idx = 0 ; idx < 3 ; idx++)
    {
        if(min_pos[idx] == max_pos[idx])
            expect = idx;
        else 
            indices.push_back(idx);
    }
    
    float resolution = 1.f / 20.f;
    for(float p1 = min_pos[indices[0]] ; p1 <= max_pos[indices[0]] ; p1 += resolution)
    {
        for(float p2 = min_pos[indices[1]] ; p2 <= max_pos[indices[1]] ; p2 += resolution)
        {
            glBegin(GL_QUADS);
            switch(expect)
            {
            case 0:
                glVertex3f(min_pos[0], p1, p2);
                glVertex3f(min_pos[0], p1 + resolution, p2);
                glVertex3f(min_pos[0], p1 + resolution, p2 + resolution);
                glVertex3f(min_pos[0], p1 , p2 + resolution);
                break;
            case 1:
                glVertex3f(p1, min_pos[1], p2);
                glVertex3f(p1 + resolution, min_pos[1], p2);
                glVertex3f(p1 + resolution, min_pos[1], p2 + resolution);
                glVertex3f(p1 , min_pos[1], p2 + resolution);
                break;
            case 2:
                glVertex3f(p1,  p2, min_pos[2]);
                glVertex3f(p1 + resolution,  p2, min_pos[2]);
                glVertex3f(p1 + resolution,  p2 + resolution, min_pos[2]);
                glVertex3f(p1 ,  p2 + resolution, min_pos[2]);
                break;
            }
            glEnd();
        }
    }
    
}

void StageRoom::pre_render()
{
    StageObject::pre_render();
}

void StageRoom::rendering()
{
    for(RoomWall q : walls)
    {
        apply_material(GL_FRONT, q.material);
        draw_high_resolution(q.points);
    }
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
