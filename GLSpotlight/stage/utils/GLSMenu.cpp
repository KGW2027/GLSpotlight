#define _CRT_SECURE_NO_WARNINGS

#include "GLSMenu.h"
#include <cassert>
#include <windows.h>
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut_std.h>
#include "../StageBuilder.h"
#include "../objects/StageObject.h"
#include "../objects/StageWaver.h"

std::vector<wchar_t*> GLSMenu::music_files;

enum
{
    STOP    = 0x01,
    MUSICS  = 0xF0,
};

void Interact(int value)
{
    if((value & MUSICS) == MUSICS)
    {
        std::wstring new_path(L"../");
        new_path += std::wstring(GLSMenu::music_files[value-MUSICS]);
        wchar_t* buffer = new wchar_t[new_path.length()+1];
        wcscpy(buffer, new_path.c_str());
            
        for(StageObject* object : StageBuilder::get_render_objects())
        {
            if(StageWaver* waver = dynamic_cast<StageWaver*>(object))
            {
                waver->stop();
                waver->play_new(buffer);
                waver->play();
            }
        }
        return;
    }

    switch(value)
    {
    case STOP:
        for(StageObject* object : StageBuilder::get_render_objects())
        {
            if(StageWaver* waver = dynamic_cast<StageWaver*>(object))
                waver->stop();
        }
        break;
    }
}


GLSMenu::GLSMenu()
{
}

void GLSMenu::RegisterMenu()
{
    GLuint musics = glutCreateMenu(Interact);

    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(L"..\\*.wav", &findFileData);

    assert(hFind != INVALID_HANDLE_VALUE);

    int music_cnt = 0;
    
    do {
        if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            wchar_t* name = new wchar_t[100];
            size_t idx = 0;
            while(idx < 100)
            {
                name[idx] = findFileData.cFileName[idx];
                if(name[idx++] == '\0') break;
            }

            char* button_display = new char[idx];
            WideCharToMultiByte(CP_UTF8, 0, name, -1, button_display, idx, nullptr, nullptr);
            glutAddMenuEntry(button_display, MUSICS + music_cnt++);
            delete[] button_display;

            music_files.push_back(name);
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    glutCreateMenu(Interact);
    glutAddMenuEntry("Stop", STOP);
    glutAddSubMenu("Musics", musics);
    
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}
