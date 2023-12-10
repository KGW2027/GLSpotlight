#pragma once

#include "StageObject.h"
#include "../../music/MusicStructs.h"

#define MUTE_DECAY 0.9
#define MIN_dB     80.0

class MusicReader;

class StageWaver : public StageObject
{
    wchar_t         *path_         ;
    MusicReader     *music_reader_ ;
    MUSIC_PROCESSOR m_processor_   ;
    double          *draw_data_    ;
    Material        mat_freq_      ;
    bool            is_terminated_ ;

    void pre_render() override;
    void rendering() override;
    void post_render() override;

    void init_processor();
    void process_frame();
    void render_3d();
    
    
public:
    StageWaver(const wchar_t* wav_path);
    void ready() override;

    void stop();
    void play();
    void play_new(wchar_t* new_path);
    
};
