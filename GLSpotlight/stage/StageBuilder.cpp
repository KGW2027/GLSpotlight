#include <gl/freeglut.h>
#include "objects/StageWaver.h"
#include "StageBuilder.h"

#define TIMER_INTERVAL 16

StageBuilder* StageBuilder::s_builder = nullptr;
std::vector<StageObject*> StageBuilder::render_objects_;

#pragma region Private GL Manage

void display()
{
    assert(StageBuilder::s_builder != nullptr);

    for(StageObject* obj : StageBuilder::get_render_objects())
    {
        obj->render();
    }
}

void interval(int v)
{
    glutPostRedisplay();
    glutTimerFunc(StageBuilder::s_builder->timer_tick, interval, v);
}

#pragma endregion 


void StageBuilder::init()
{
    waver = new StageWaver(L"../test5.wav");
}

StageBuilder::StageBuilder(int argc, char* argv[])
{
    // OpenGL 초기화 
    glutInit(&argc, argv);
    glutInitWindowSize(1280, 720);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutCreateWindow("GLSpotlight - Beta");

    set_fps(30);
    init();
    
    s_builder = this;
    glutDisplayFunc(display);
    glutTimerFunc(timer_tick, interval, 0);
    
}

/**
 * \brief 프로그램을 시작하는 함수
 */
void StageBuilder::start()
{
    // Spectrum Render 오브젝트 실행
    add_render_objects(waver);

    

    // OpenGL 액션 시작
    glutMainLoop();
}
