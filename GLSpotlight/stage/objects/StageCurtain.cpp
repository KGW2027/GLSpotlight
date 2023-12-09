#include "StageCurtain.h"

#include <cmath>

#include "../textures/TextureBase.h"

#define TEXTURE_KEY "Curtain"


void StageCurtain::apply_points()
{
    // 가로축은 X, 전후축은 Y
    // X축 값은 col, points[0] 값에 의존 (get_start_of_x() + get_sign * col + points[row * col][0])
    // Y축 값은 points[1]      값에 의존 (-2 + points[row * col][1])
    // Z축 값은 row            값에 의존 (2 - (0.1 * row))
    
    size_t row = 0, col = 0;
    float dx    = get_gap_of_x()   * get_sign();
    float dz    = get_gap_of_h();
    float sx    = get_start_of_x() * get_sign();
    float sz    = get_start_of_h();
    while(row < range_[0])
    {
        size_t idx = get_matrix_idx(row, col);
        quad_matrix_[idx][0] =  sx   + static_cast<float>(col) * dx + points[idx][0];
        quad_matrix_[idx][1] = -2.f  + points[idx][1];
        quad_matrix_[idx][2] =  sz   - static_cast<float>(row) * dz;
        
        if(++col >= range_[1])
        {
            row++;
            col = 0;
        }
    }
}

void StageCurtain::draw_points()
{
    load_texture(false, tid_curtain_);
    
    apply_points();
    float coord_height = 1.f / static_cast<float>(range_[0]);
    float coord_side   = 1.f / static_cast<float>(range_[1]);
    
    for(size_t row = 0 ; row < range_[0] - 1 ; row++)
    {
        for(size_t col = 0 ; col < range_[1] - 1 ; col++)
        {
            glBegin(GL_QUADS);
            
            for(int pidx = 0 ; pidx < 4 ; pidx++)
            {
                size_t rr = row + ((pidx & 2) >> 1);
                size_t rc = col + ((pidx & 2) >> 1 ^ (pidx & 1));
                
                vec3 tgt = quad_matrix_[get_matrix_idx(rr, rc)];
                glTexCoord2f(static_cast<float>(rc) * coord_height, static_cast<float>(rr) * coord_side);
                glVertex3f(tgt[0], tgt[1], tgt[2]);
            }
            
            glEnd();
        }
    }
    unload_texture();
}

void StageCurtain::pre_render()
{
    StageObject::pre_render();
}

void StageCurtain::rendering()
{
    StageObject::rendering();

    draw_points();
}

void StageCurtain::post_render()
{
    StageObject::post_render();
}

void StageCurtain::ready()
{
    StageObject::ready();

    tid_curtain_ = TextureBase::get_texture(TEXTURE_KEY);

    // 커튼 곡선 초기화
    // Range_ { Z 길이(높이), X 길이(좌우 길이) }
    range_ = new size_t[2]
    {
        static_cast<size_t>((get_start_of_h() - get_end_of_h()) / 0.05f),
        static_cast<size_t>(std::ceil(abs(get_end_of_x() - get_start_of_x()) / 0.05f))
    };
    size_t range = range_[0] * range_[1];
    points = new float*[range];
    for(size_t i = 0 ; i < range ; i++)
    {
        points[i] = new float[2];
        points[i][0] = 0.0f;
        points[i][1] = static_cast<float>(sin(i % range_[1])) / 10.f;
    }
    quad_matrix_ = new vec3[range];

    printf("Curtain Size : (%llu * %llu)\n", range_[0], range_[1]);

    // X축을 Y번 쌓는 일차원 배열로 계산
    
    
}
