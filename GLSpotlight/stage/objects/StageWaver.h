#pragma once
#include "StageObject.h"
#include "../../music/MusicReader.h"

typedef double** frequencies;
typedef double*  draw_data;

#define MUTE_DECAY 0.9
#define MIN_dB     80.0

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
    const wchar_t   *path_         ;
    MusicReader     *music_reader_ ;
    music_processor m_processor_   ;
    draw_data       draw_data_     ;
    
    

    void pre_render() override;
    void rendering() override;
    void post_render() override;

    void init_processor();
    void process_frame();
    
public:
    StageWaver(const wchar_t* wav_path);
    void ready() override;
    
};
