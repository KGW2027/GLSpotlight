#include <gl/freeglut.h>
#include "objects/StageWaver.h"
#include "StageBuilder.h"

#include "objects/StageRoom.h"
#include "objects/StageSpotlight.h"
#include "utils/GLSCamera.h"

#define TIMER_INTERVAL 16

StageBuilder* StageBuilder::s_builder = nullptr;
std::vector<StageObject*> StageBuilder::render_objects_;
GLSCamera* StageBuilder::camera_ = nullptr;
std::vector<StageSpotlight*> StageBuilder::render_lights_;

#pragma region Private GL Manage

void display()
{
    assert(StageBuilder::s_builder != nullptr);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    StageBuilder::get_camera()->update();
    
    for(StageObject* obj : StageBuilder::get_render_objects())
    {
        glColor3f(1, 1, 1);
        obj->render();
    }

    glutSwapBuffers();
}

void interval(int v)
{
    glutPostRedisplay();
    glutTimerFunc(StageBuilder::s_builder->timer_tick, interval, v);
}

void mouse_move_event(int arg1, int arg2)
{
    StageBuilder::get_camera()->mouse_move(arg1, arg2);
}

void mouse_click_event(int arg1, int arg2, int arg3, int arg4)
{
    StageBuilder::get_camera()->mouse_click(arg1, arg2, arg3, arg4);
}

void mouse_wheel_event(int arg1, int arg2, int arg3, int arg4)
{
    StageBuilder::get_camera()->mouse_wheel(arg1, arg2, arg3, arg4);
}

#pragma endregion 


void StageBuilder::init()
{
    // waver = new StageWaver(L"../test5.wav");
    camera_ = new GLSCamera();
}

StageBuilder::StageBuilder(int argc, char* argv[])
{
    // OpenGL 초기화 
    glutInit(&argc, argv);
    glutInitWindowSize(1280, 720);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow("GLSpotlight - Beta");

    set_fps(30);
    init();
    
    s_builder = this;
    
    glutDisplayFunc(display);
    glutMouseFunc(mouse_click_event);
    glutMotionFunc(mouse_move_event);
    glutMouseWheelFunc(mouse_wheel_event);
    glutTimerFunc(timer_tick, interval, 0);
    
}

void StageBuilder::start()
{
    
    // Building Wall Add
    add_render_objects(new StageRoom());
    
    // Spectrum Render 오브젝트 실행
    // add_render_objects(waver);

    // Light Sources
    render_lights_.push_back(new StageSpotlight(GL_LIGHT0));
    render_lights_.push_back(new StageSpotlight(GL_LIGHT1));
    render_lights_.push_back(new StageSpotlight(GL_LIGHT2));
    render_lights_[0]->add_position(0, -1.8f, 1.8f);
    render_lights_[1]->add_position(2, -1.8f, 1.8f);
    render_lights_[2]->add_position(-2, -1.8f, 1.8f);
    add_render_objects(render_lights_[0]);
    add_render_objects(render_lights_[1]);
    add_render_objects(render_lights_[2]);

    // OpenGL 액션 시작

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_LIGHTING);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    
    glutMainLoop();
}
