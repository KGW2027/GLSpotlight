#include "GLSCamera.h"

#include <windows.h>
#include <gl/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#define MOUSE_SENSIVITY 30.0
#define MOUSE_WHEEL_SENSIVITY 0.03


void GLSCamera::rotate_camera(float delta, bool is_x)
{
    vec4 tmpfwd = vec4(forward_, 0);
    vec4 tmpup  = vec4(up_, 0);
    mat4 rot    = rotate(mat4(1), glm::radians(delta), is_x ? up_ : right_);

    forward_    = rot * tmpfwd;
    up_         = rot * tmpup; 
    // at_           = eye_ + forward_ * arm;
    eye_          = at_ - forward_ * arm;
    update_state();
}

void GLSCamera::update()
{
    int width   = glutGet(GLUT_WINDOW_WIDTH);
    int height  = glutGet(GLUT_WINDOW_HEIGHT);
    
    float x_move = - MOUSE_SENSIVITY * (move_end_[0] - move_start_[0]) / width ;
    float y_move =   MOUSE_SENSIVITY * (move_end_[1] - move_start_[1]) / height;
    rotate_camera(x_move, true);
    rotate_camera(y_move, false);
    move_start_ = move_end_;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, width * 1.0 / height, 0.1, 20);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye_[0], eye_[1], eye_[2], at_[0], at_[1], at_[2], up_[0], up_[1], up_[2]);

    glBegin(GL_LINES);
    
    glColor3f(1, 0, 0);
    glVertex3f(eye_[0], eye_[1], eye_[2]);
    glVertex3f(at_[0], at_[1], at_[2]);
    glEnd();
}

void GLSCamera::mouse_click(int button, int state, int x, int y)
{
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        move_start_ = {x, y};
        move_end_   = {x, y};
    }
}

void GLSCamera::mouse_move(int x, int y)
{
    move_end_ = {x, y};
}

void GLSCamera::mouse_wheel(int unk, int direction, int x, int y)
{
    arm += direction * MOUSE_WHEEL_SENSIVITY;
}
