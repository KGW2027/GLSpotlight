#define _CRT_SECURE_NO_WARNINGS

#include "StageWaver.h"
#include "../../music/MusicReader.h"
#include <thread>
#include <glm/common.hpp>
#include "../StageBuilder.h"


StageWaver::StageWaver(const wchar_t* wav_path) : music_reader_(nullptr), m_processor_(), draw_data_(nullptr)
{
    path_ = new wchar_t[128];
    wcscpy(path_, wav_path);
    music_reader_ = new MusicReader(path_);
}

void StageWaver::ready()
{
    position_[1] = -3.95f;
    position_[2] = 0.6f;
    scale_ = {.8f, .8f, .8f};

    play();
    
    // Background
    Material mat_back = get_default_material();
    mat_back.color = get_rgba_by_ubyte(0, 0, 0, 1.0);
    meshes_.push_back(Mesh{
        make_quad(-3.5, 0, -1, 3.5, 0, 1), mat_back
    });

    mat_freq_ = get_default_material();
    mat_freq_.color = get_rgba_by_ubyte(182, 231, 255, 1.0f);
}

void StageWaver::stop()
{
    is_terminated = true;
    music_reader_->terminate();
    StageBuilder::set_curtain_mode(false);
}

void StageWaver::play()
{
    while(!music_reader_->is_ready());
    
    is_terminated = false;
    StageBuilder::set_curtain_mode(true);
    
    // Play Spectrum Render
    init_processor();
    std::thread frame_proc(&StageWaver::process_frame, this);
    frame_proc.detach();

    // Play Music
    assert(music_reader_ != nullptr);

    std::thread music_thread(&MusicReader::play_music, music_reader_);
    music_thread.detach();
}

void StageWaver::play_new(wchar_t* new_path)
{
    wcscpy(path_, new_path);
    free(new_path);
    music_reader_->set_path(path_);
}

void StageWaver::pre_render()
{
    StageObject::pre_render();
}

void StageWaver::rendering()
{
    // Freq dB 정보 갱신
    if(m_processor_.index < m_processor_.shape[0] && !is_terminated)
    {
        double* cur_freq = m_processor_.freq[m_processor_.index];
        for(uint i = 0 ; i < m_processor_.shape[1] ; i++)
        {
            if( abs(MIN_dB + cur_freq[i]) < 0.01 ) draw_data_[i] *= MUTE_DECAY;
            else draw_data_[i] = cur_freq[i];
        }
    }
    // 음악이 종료된 경우, 모든 값에 DECAY 해서 0으로 만듬.
    else
    {
        for(uint i = 0 ; i < m_processor_.shape[1] ; i++) draw_data_[i] *= MUTE_DECAY;
    }

    // render_2d();
    render_3d();
}

void StageWaver::post_render()
{
    StageObject::post_render();
}

void StageWaver::init_processor()
{
    m_processor_.index = 0;
    m_processor_.shape = new UINT32[2];
    m_processor_.freq  = music_reader_->output(m_processor_.shape, &m_processor_.term, &m_processor_.length);
    draw_data_ = new double[m_processor_.shape[1]];
}

void StageWaver::process_frame()
{
    typedef std::chrono::high_resolution_clock hr_clock;
    auto start_time = hr_clock::now();
    auto until_time = hr_clock::now();
    auto delay      = std::chrono::nanoseconds(static_cast<UINT64>(m_processor_.length / m_processor_.shape[0]) * 100);
    do
    {
        std::this_thread::sleep_until(until_time += delay);
        m_processor_.current_time = std::chrono::duration_cast<std::chrono::milliseconds>(until_time - start_time).count();
    }while(++m_processor_.index < m_processor_.shape[0] && !is_terminated);

    m_processor_.index = 0;
}

void StageWaver::render_3d()
{
    apply_material(GL_FRONT_AND_BACK, mat_freq_);
    float dx = 9.0f / static_cast<float>(m_processor_.shape[1]);
    for(uint i = 0 ; i < m_processor_.shape[1] ; i++)
    {
        float norm_value = glm::clamp(abs(static_cast<float>(draw_data_[i] / 80.)), 0.f, 1.f) * 0.9f;
        float sx = -4.5f + dx * i, ex = sx + dx;
        quad q = make_quad(sx, 0.01f, -norm_value, ex, 0.01f, norm_value);
        draw_quad(q);
    }
}