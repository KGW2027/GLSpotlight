#include <gl/freeglut.h>

#include "renderer/GLSCircle.h"

GLSCircle circular;

void init();
void display();

int main(int argc, char* argv[])
{
    
    glutInit(&argc, argv);
    glutInitWindowSize(1280, 720);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_SINGLE | GLUT_RGBA);
    glutCreateWindow("GLSpotlight - Beta");
    
    init();
    
    glutDisplayFunc(display);
    glutMainLoop();

    
    return 0;
}

void display()
{
    circular.render();
}

void init()
{
    // circular = GLSCircle();
}