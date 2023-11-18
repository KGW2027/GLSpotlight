#include <cstdio>
#include <gl/freeglut.h>

#include "renderer/GLSCircle.h"

GLSCircle circular;

void init();
void display();
void timer(int value);
void redraw();

int main(int argc, char* argv[])
{
    
    glutInit(&argc, argv);
    glutInitWindowSize(1280, 720);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutCreateWindow("GLSpotlight - Beta");
    
    init();
    
    glutDisplayFunc(display);
    glutTimerFunc(100, timer, 0);
    glutMainLoop();

    
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

void redraw()
{
    circular.set_random_radius();
    glutPostRedisplay();
}

void timer(int value)
{
    redraw();
    glutTimerFunc(33, timer, 0);
}