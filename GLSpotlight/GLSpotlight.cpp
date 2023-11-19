#include <cstdio>
#include <gl/freeglut.h>

#include "renderer/GLSCircle.h"
#include "MusicReader.h"

GLSCircle circular;

void init();
void display();
void timer(int value);
void redraw();

void callback(float** array, int window)
{
    float* circle_weight = new float[CIRCULAR_PRECISION];
    int range = static_cast<int>(ceil(static_cast<float>(window) / static_cast<float>(CIRCULAR_PRECISION)));
    for(int i = 0 ; i < window ; i++)
        circle_weight[i/range] += (*array)[i];
    for(int i = 0 ; i < CIRCULAR_PRECISION ; i++)
        circle_weight[i] /= range;

    circular.set_radius(circle_weight);
    glutPostRedisplay();
    printf("Redisplay\n");
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(1280, 720);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutCreateWindow("GLSpotlight - Beta");
    
    init();
    
    glutDisplayFunc(display);
    // glutTimerFunc(100, timer, 0);
    glutMainLoop();

    const wchar_t* filename = L"../test.wav";
    MusicReader reader = MusicReader(filename);
    reader.play_music(callback);
    
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

// void redraw()
// {
//     circular.set_random_radius();
//     glutPostRedisplay();
// }

// void timer(int value)
// {
//     redraw();
//     glutTimerFunc(33, timer, 0);
// }