
#include "StageWaver.h"

#include <thread>
#include <gl/freeglut.h>


StageWaver::StageWaver(const wchar_t* wav_path) : music_reader_(nullptr), m_processor_(), draw_data_(nullptr)
{
    path_ = wav_path;
    music_reader_ = new MusicReader(path_);
}

void StageWaver::ready()
{
    position_[0] -= 0.5f;
    scale_ = {.8f, .8f, .8f};
    
    // Play Spectrum Render
    init_processor();
    std::thread frame_proc(&StageWaver::process_frame, this);
    frame_proc.detach();

    // Play Music
    assert(music_reader_ != nullptr);
    
    std::thread music_thread(&MusicReader::play_music, music_reader_);
    music_thread.detach();
}

void StageWaver::pre_render()
{
    StageObject::pre_render();
}

void StageWaver::rendering()
{
    // Freq dB 정보 갱신
    if(m_processor_.index < m_processor_.shape[0])
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
    }while(++m_processor_.index < m_processor_.shape[0]);
}

void StageWaver::render_3d()
{
    glBegin(GL_QUADS);

    glVertex3f(-1, -1, -1);
    glVertex3f(1, -1, -1);
    glVertex3f(1, -1, 1);
    glVertex3f(-1, -1, 1);
    
    glEnd();
}

void StageWaver::render_2d()
{
    // Screen Rendering (2D)
    glLineWidth(.5);
    color_rgb(182, 231, 255);
    
    glBegin(GL_LINES);
    for(uint i = 0 ; i < m_processor_.shape[1] ; i++)
    {
        float shift = static_cast<float>(window_size_[0]) * 0.05f + static_cast<float>(i);
        glm::vec4 pos(shift, static_cast<float>(window_size_[1]) / 2.0 - draw_data_[i]* -1.5, 0, 1);
        glut_pos_to_gl_pos(&pos);
        glVertex2f(pos[0], pos[1]);
        glVertex2f(pos[0], -pos[1]);
    }
    glEnd();
}
