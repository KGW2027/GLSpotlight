#define _CRT_SECURE_NO_WARNINGS

#include "StageWaver.h"
#include "../../music/MusicReader.h"
#include <thread>
#include <gl/freeglut.h>
#include <glm/common.hpp>
#include "../StageBuilder.h"


StageWaver::StageWaver() : music_reader_(nullptr), m_processor_(), draw_data_(nullptr),
                                                  mat_freq_(),
                                                  is_terminated_(false)
{
    path_ = new wchar_t[128];
    // wcscpy(path_, wav_path);
    music_reader_ = new MusicReader();
}

void StageWaver::ready()
{
    position_[1] = -3.95f;
    position_[2] = 0.6f;
    scale_ = {.8f, .8f, .8f};
    
    // Background
    Material mat_back = get_default_material();
    mat_back.color = get_rgba_by_ubyte(0, 0, 0, 1.0);
    meshes_.push_back(Mesh{
        make_quad(-3.5, 0, -1, 3.5, 0, 1), mat_back
    });

    mat_freq_ = get_default_material();
    mat_freq_.color    = get_rgba_by_ubyte(182, 231, 255, 1.0f);
    mat_freq_.specular = get_rgba_by_ubyte(184, 248, 251, 1.0f);
    // mat_freq_.shininess= 0.01f;
}

void StageWaver::stop()
{
    is_terminated_ = true;
    music_reader_->terminate();
    m_processor_.debug.clear();
    StageBuilder::set_curtain_mode(false);
}

void StageWaver::play()
{
    while(!music_reader_->is_ready()) {}
    
    is_terminated_ = false;
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
    delete[] new_path;
    music_reader_->set_path(path_);
}

void StageWaver::pre_render()
{
    if(m_processor_.debug.size() > 0)
    {
        glColor3f(1, 1, 1);
        float raster_z = 1.9f;
        for(std::string line : m_processor_.debug)
        {
            glRasterPos3f(3.1f, -1.5f, raster_z);
            for(size_t idx = 0 ; idx < line.length() ; idx++)
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, line[idx]);
            raster_z -= 0.09f;
        }
    }
    
    StageObject::pre_render();
}

void StageWaver::rendering()
{
    // Freq dB 정보 갱신
    if(m_processor_.index < m_processor_.time_len && !is_terminated_)
    {
        double* cur_freq = m_processor_.data[m_processor_.index];
        for(size_t i = 0 ; i < m_processor_.win_len ; i++)
        {
            if( abs(cur_freq[i]) >= MIN_dB - 0.1f ) draw_data_[i] *= MUTE_DECAY;
            else draw_data_[i] = cur_freq[i];
        }
    }
    // 음악이 종료된 경우, 모든 값에 DECAY 해서 0으로 만듬.
    else
    {
        for(size_t i = 0 ; i < m_processor_.win_len ; i++) draw_data_[i] *= MUTE_DECAY;
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
    m_processor_ = music_reader_->processor_;
    draw_data_   = new double[m_processor_.win_len];
}

void StageWaver::process_frame()
{
    typedef std::chrono::high_resolution_clock hr_clock;
    auto until_time = hr_clock::now();
    auto delay      = std::chrono::nanoseconds(m_processor_.length / m_processor_.time_len * 100);

    for(size_t idx = 0 ; idx < m_processor_.win_len ; idx++) draw_data_[idx] = -MIN_dB;
    
    do   std::this_thread::sleep_until(until_time += delay);
    while(++m_processor_.index < m_processor_.time_len && !is_terminated_);

    m_processor_.index = 0;
}

void StageWaver::render_3d()
{
    apply_material(GL_FRONT_AND_BACK, mat_freq_);
    float  dx    = 9.0f / static_cast<float>(m_processor_.win_len);
    float  half  = static_cast<float>(m_processor_.win_len) / 2.f;
    size_t uhalf = m_processor_.win_len / 2;
    for(size_t i = 0 ; i < m_processor_.win_len ; i++)
    {
        float blue = glm::clamp(pow(abs(static_cast<float>(i) - half) / half, 3), 0., 1.);
        glColor3f(1.0f-sqrt(blue), blue, blue);

        size_t cvt_i = i <= uhalf ? uhalf - i : m_processor_.win_len - i + uhalf;
        float norm_value = glm::clamp(abs(static_cast<float>(draw_data_[cvt_i] / 80.)), 0.f, 1.f) * 0.9f;
        float sx = -4.5f + dx * i, ex = sx + dx;
        quad q = make_quad(sx, 0.01f, -norm_value, ex, 0.01f, norm_value);
        draw_quad(q);
    }
}