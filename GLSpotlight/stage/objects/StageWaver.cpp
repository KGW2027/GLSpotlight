
#include "StageWaver.h"

#include <thread>


StageWaver::StageWaver(const wchar_t* wav_path) : music_reader_(nullptr), wav_renderer_(nullptr)
{
    path_ = wav_path;
    music_reader_ = new MusicReader(path_);
    wav_renderer_ = new GLSCircle(200.0f);
    wav_renderer_->set_music_reader(music_reader_);
}

void StageWaver::start()
{
    // Play Spectrum Render
    assert(wav_renderer_ != nullptr);

    std::thread render_thread(&GLSCircle::play, wav_renderer_);
    render_thread.detach();

    // Play Music
    assert(music_reader_ != nullptr);
    
    std::thread music_thread(&MusicReader::play_music, music_reader_);
    music_thread.detach();
}

void StageWaver::pre_render()
{
    
}

void StageWaver::rendering()
{
    wav_renderer_->render();
}

void StageWaver::post_render()
{
    
}