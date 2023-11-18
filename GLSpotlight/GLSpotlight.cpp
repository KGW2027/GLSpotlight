#include <glm/glm.hpp>
#include <GL/freeglut.h>
#include "MusicReader.h"


#define EXIT_FILE_IS_NOT_EXISTS 0xF0000001



void normalize(double** array, int len_1d, int len_2d) {
    double max_value = -1;
    double min_value = 999;
    
    for (int d1 = 0; d1 < len_1d; d1++)
        for (int d2 = 0; d2 < len_2d; d2++) {
            max_value = std::max(max_value, array[d1][d2]);
            min_value = std::min(min_value, array[d1][d2]);
            if(array[d1][d2] < -100 || array[d1][d2] > 100)
                break;
        }

    for (int d1 = 0; d1 < len_1d; d1++)
        for (int d2 = 0; d2 < len_2d; d2++)
            array[d1][d2] = (array[d1][d2] - min_value) / (max_value - min_value);
}

int main(int argc, char** argv) {
    const wchar_t* filename = L"../test.mp3";
    MusicReader reader = MusicReader(filename);
    
    return 0;
}

// using namespace glm;
//
//
//
// void display()
// {
//     glClear(GL_COLOR_BUFFER_BIT);
//
//     
//     
//     glFlush();
// }
//
//
// int main(int argc, char** argv)
// {
//     glutInit(&argc, argv);
//     glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE | GLUT_DEPTH);
//     glutInitWindowSize(800, 600);
//     glutCreateWindow("GLSpotlight");
//     glutDisplayFunc(display);
//     
//     glutMainLoop();
//     
//     return 0;
// }
