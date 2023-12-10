#pragma once

#include "StageObject.h"

typedef double** frequencies;
typedef double*  draw_data;

#define MUTE_DECAY 0.9
#define MIN_dB     80.0

typedef unsigned int UINT32;
typedef long long LONGLONG;

class MusicReader;

struct music_processor
{
    UINT32      index;
    UINT32      *shape;
    frequencies freq;
    LONGLONG    term;
    LONGLONG    length;
    LONGLONG    current_time;
};

class StageWaver : public StageObject
{
    wchar_t             *path_         ;
    MusicReader         *music_reader_ ;
    music_processor     m_processor_   ;
    draw_data           draw_data_     ;
    Material            mat_freq_      ;
    bool                is_terminated  ;

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
