#pragma once
#include "StageObject.h"
#include "../../music/MusicReader.h"
#include "../../renderer/GLSCircle.h"

class StageWaver : public StageObject
{
    const wchar_t   *path_         ;
    MusicReader     *music_reader_ ;
    GLSCircle       *wav_renderer_  ;
    

    void pre_render() override;
    void rendering() override;
    void post_render() override;
    
public:
    StageWaver(const wchar_t* wav_path);
    void start();
    
};
