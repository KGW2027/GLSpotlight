
#include <gl/glew.h>
#include <gl/freeglut.h>

#include "objects/StageWaver.h"
#include "objects/StageRoom.h"
#include "objects/StageSpotlight.h"
#include "utils/GLSCamera.h"
#include "StageBuilder.h"

#include <ctime>
#include <random>

#include "objects/StageCurtain.h"
#include "objects/StageHuman.h"
#include "objects/StageStage.h"
#include "textures/TextureBase.h"
#include "utils/GLSMenu.h"

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

    // glUseProgram(StageBuilder::shader_->get_program_id());
    
    StageBuilder::get_camera()->update();
    
    for(StageObject* obj : StageBuilder::get_render_objects())
    {
        glColor3f(1, 1, 1);
        obj->render();
    }

    // glUseProgram(0);
    
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
    for(StageObject* object : StageBuilder::get_render_objects())
    {
        if(GLSPhysics* physObj = dynamic_cast<GLSPhysics*>(object))
        {
            physObj->move_force(ivec2(arg1, arg2));
        }
    }
}

void mouse_click_event(int arg1, int arg2, int arg3, int arg4)
{
    StageBuilder::get_camera()->mouse_click(arg1, arg2, arg3, arg4);

    if(arg1 == GLUT_LEFT_BUTTON)
    {
        for(StageObject* object : StageBuilder::get_render_objects())
        {
            if(GLSPhysics* physObj = dynamic_cast<GLSPhysics*>(object))
            {
                if(arg2 == GLUT_DOWN)
                    physObj->start_physics(ivec2(arg3, arg4));
                else if(arg2 == GLUT_UP)
                    physObj->end_physics();
            }
        }
    }
}

void mouse_wheel_event(int arg1, int arg2, int arg3, int arg4)
{
    StageBuilder::get_camera()->mouse_wheel(arg1, arg2, arg3, arg4);
}

void keyboard_event(unsigned char arg1, int arg2, int arg3)
{
    switch(arg1)
    {
    case '1':
        StageBuilder::set_curtain_mode(true);
        break;
    case '2':
        StageBuilder::set_curtain_mode(false);
        break;
    case '3':
        for(StageObject* object : StageBuilder::get_render_objects())
        {
            if(StageWaver* waver = dynamic_cast<StageWaver*>(object))
                waver->stop();
        }
        break;
    }
}

#pragma endregion 


void StageBuilder::init()
{
    waver = new StageWaver();
    camera_ = new GLSCamera();
}

float StageBuilder::get_random(float min, float max)
{
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<float> distribution(min, max);

    // Generate a random value between x1 and x2
    return distribution(generator);
}

StageBuilder::StageBuilder(int argc, char* argv[])
{
    // OpenGL 초기화 
    glutInit(&argc, argv);
    glutInitWindowSize(1280, 720);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow("GLSpotlight - Beta");
    
    s_builder = this;
    set_fps(30);
    init();
    
    glutDisplayFunc(display);
    glutMouseFunc(mouse_click_event);
    glutMotionFunc(mouse_move_event);
    glutMouseWheelFunc(mouse_wheel_event);
    glutKeyboardFunc(keyboard_event);
    glutTimerFunc(timer_tick, interval, 0);
    
}

void StageBuilder::load_textures()
{
    TextureBase::load_texture("../textures/stage.png", "Stage");
    TextureBase::load_texture("../textures/spotlight.png", "Spotlight");
    TextureBase::load_texture("../textures/metal.png", "Metal");
    TextureBase::load_texture("../textures/room.png", "Room");
    TextureBase::load_texture("../textures/curtain.png", "Curtain");
}

void StageBuilder::load_objects()
{
    
    // Building Wall Add
    add_render_objects(new StageRoom());
    add_render_objects(new StageStage());

    // Curtain Add
    StageCurtain *left = new StageCurtain(), *right = new StageCurtain();
    left->set_direction(false); right->set_direction(true);
    add_render_objects(left);
    add_render_objects(right);
    
    // Spectrum Render 오브젝트 실행
    add_render_objects(waver);

    // Light Sources
    render_lights_.push_back(new StageSpotlight(GL_LIGHT0));
    render_lights_.push_back(new StageSpotlight(GL_LIGHT1));
    render_lights_.push_back(new StageSpotlight(GL_LIGHT2));
    render_lights_[0]->add_position(0, -1.8f, 1.6f);
    render_lights_[1]->add_position(2, -1.8f, 1.6f);
    render_lights_[2]->add_position(-2, -1.8f, 1.6f);
    add_render_objects(render_lights_[0]);
    add_render_objects(render_lights_[1]);
    add_render_objects(render_lights_[2]);

    // Crowd
    // 군중들은 x {-3.5 ~ 3.5} y {-1.5 ~ 1.5} z {-.75} 에 위치한다.
    size_t total_human = 0;
    float x, y = -1.5f + get_random(0.3f, 0.5f);
    for(; y <= 1.5f ;)
    {
        x = -3.5f + get_random(0.3f, 0.5f);
        for( ; x <= 3.5f ; )
        {
            add_render_objects(new StageHuman(x, y + get_random(-0.05f, 0.05f)));
            total_human++;
            x += get_random(0.3f, 0.5f);
        }
        y += get_random(0.3f, 0.5f);
    }
    printf("%lld Human placed.\n", total_human);
    
}

void StageBuilder::start()
{
    load_textures();
    load_objects();

    // OpenGL 액션 시작
    glewInit();

    menu = new GLSMenu();
    menu->RegisterMenu();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS);
    
    glEnable(GL_LIGHTING);

    glColorMaterial(GL_FRONT, GL_AMBIENT);
    glEnable(GL_COLOR_MATERIAL);
    
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    
    glutMainLoop();
}

void StageBuilder::add_render_objects(StageObject* obj)
{
    obj->ready();
    render_objects_.push_back(obj);
}

void StageBuilder::set_curtain_mode(bool is_opened)
{
    for(StageObject* object : get_render_objects())
    {
        if(StageCurtain* curtain = dynamic_cast<StageCurtain*>(object))
        {
            if(is_opened) curtain->open_curtain();
            else          curtain->close_curtain();  
        }
    }
}

