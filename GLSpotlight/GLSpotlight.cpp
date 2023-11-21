#include <thread>
#include <gl/freeglut.h>

#include "renderer/GLSCircle.h"
#include "music/MusicReader.h"

GLSCircle circular;
MusicReader music_reader;

void init()
{
    circular = GLSCircle();
}

void initMusic();
void redisplay(int v);
void display();

/*
 * TODO - dB 데이터가 제대로 적용되지 않은 것으로 보임.
 * 이에 대해, STFT쪽이나 music parsing 쪽에서 재점검필요.
 * test  - 18988 / 3분 21초 (201초)
 * test2 - 28040 / 4분 59초 (299초)
 * test3 - 12033 / 2분 19초 (140초)
 * test4 - 25423 / 4분 31초 (271초)
 */

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(1280, 720);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutCreateWindow("GLSpotlight - Beta");
    
    init();
    initMusic();
    
    glutDisplayFunc(display);
    glutTimerFunc(50, redisplay, 0);
    glutMainLoop();
    
    return 0;
}

#pragma region Screen_Display

void display()
{
    circular.render();
}

void redisplay(int v)
{
    glutPostRedisplay();
    glutTimerFunc(90, redisplay, v);
}

#pragma endregion 

#pragma region Music_Management

void initMusic()
{
    const wchar_t* filename = L"../test2.wav";
    music_reader = MusicReader(filename);
    circular.set_music_reader(&music_reader);
    std::thread render_thread(&GLSCircle::play, &circular);
    std::thread music_thread(&MusicReader::play_music, &music_reader);
    render_thread.detach();
    music_thread.detach();
}

#pragma endregion
