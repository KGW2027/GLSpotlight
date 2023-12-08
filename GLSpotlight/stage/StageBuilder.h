#pragma once

#include <vector>
#include "objects/StageObject.h"

class StageSpotlight;
class GLSCamera;
class StageWaver;

class StageBuilder
{
    
    void init();

    static std::vector<StageObject*>    render_objects_;
    static GLSCamera                    *camera_;
    static std::vector<StageSpotlight*> render_lights_;          
    
public:
    
    static StageBuilder *s_builder;
           StageWaver   *waver;
           unsigned int timer_tick;

    StageBuilder(int argc, char *argv[]);

    void start();
    void set_fps(unsigned int fps) { timer_tick = 1000 / fps; }

    static std::vector<StageObject*> get_render_objects() { return render_objects_; }
    static void add_render_objects(StageObject* obj) { obj->ready(); render_objects_.push_back(obj); }
    static GLSCamera* get_camera() { return camera_; }
};
