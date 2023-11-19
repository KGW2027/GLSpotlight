#include <cstdio>
#include <gl/freeglut.h>

#include "renderer/GLSCircle.h"
#include "MusicReader.h"

GLSCircle circular;
std::thread music_thread;

void init();
void initMusic();
void redisplay(int v);
void display();

void callback(float** array, int window)
{
    float* circle_weight = new float[CIRCULAR_PRECISION]{0.0f};
    int range = static_cast<int>(ceil(static_cast<float>(window) / static_cast<float>(CIRCULAR_PRECISION)));
    for(int i = 0 ; i < window ; i++)
        circle_weight[i/range] += (*array)[i];
    for(int i = 0 ; i < CIRCULAR_PRECISION ; i++)
        circle_weight[i] /= range;
    
    circular.set_radius(circle_weight);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(1280, 720);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutCreateWindow("GLSpotlight - Beta");
    
    init();
    std::thread musicThreadObj(initMusic);
    
    glutDisplayFunc(display);
    glutTimerFunc(50, redisplay, 0);
    glutMainLoop();

    printf("Hello, world");
    
    return 0;
}

void display()
{
    circular.render();
}

void init()
{
    circular = GLSCircle();
}

void redisplay(int v)
{
    glutPostRedisplay();
    glutTimerFunc(50, redisplay, v);
}

void initMusic()
{
    const wchar_t* filename = L"../test.wav";
    MusicReader reader = MusicReader(filename);
    reader.play_music(callback);
}