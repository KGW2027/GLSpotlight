#include "StageStage.h"

#include "../textures/TextureBase.h"

#define TEXTURE_KEY "Stage"

void StageStage::pre_render()
{
    StageObject::pre_render();
}

void StageStage::rendering()
{
    
    // Draw Room
    load_texture(false, tid_stage);
    draw_meshes(meshes_, 20);
    unload_texture();

}

void StageStage::post_render()
{
    StageObject::post_render();
}

void StageStage::ready()
{
    StageObject::ready();

    Material mat_stage = get_default_material();
    mat_stage.color   = new GLfloat[3]{0, 122, 0};
    mat_stage.ambient = new GLfloat[4]{0.5f, 0.5f, 0.5f, 1.0f}; 
    
    // Stage-Bottom Up
    meshes_.push_back(Mesh{ // Stage Step
         make_quad(4, -2, -1, -4, -2, -.5), mat_stage
    });
    meshes_.push_back(Mesh{ // Stage Floor
         make_quad(4, -2, -.5, -4, -4, -.5), mat_stage
    });

    tid_stage = TextureBase::get_texture(TEXTURE_KEY);
}
