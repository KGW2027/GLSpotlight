#include "StageCurtain.h"

#include <cmath>
#include <thread>
#include <gl/freeglut_std.h>

#include "../textures/TextureBase.h"

#define TEXTURE_KEY     "Curtain"
#define FORCE_DECAY     0.4
#define MIN_DISTANCE    0.01f
#define CURTAIN_CLOSE   0.05f
#define CURTAIN_OPEN    2.00f

#pragma region Draw

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

#pragma endregion Draw

#pragma region Virtual

void StageCurtain::pre_render()
{
    if(is_opened_) curtain_start_ += (CURTAIN_OPEN - get_start_of_x()) * 0.03;
    else curtain_start_ -= (get_start_of_x() - CURTAIN_CLOSE) * 0.03;
        
    if(is_physics_enabled())
        hit_test();
    
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
    decay_force();
    solve_force();
}

void StageCurtain::ready()
{
    StageObject::ready();

    is_opened_ = false;
    curtain_start_ = CURTAIN_CLOSE;
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

    // 커튼 머테리얼 초기화
    mat_curtain_ = get_default_material();
    mat_curtain_.ambient = get_rgba_by_ubyte(255, 0, 0, 1.0);
    mat_curtain_.diffuse = get_rgba_by_ubyte(255, 0, 0, 1.0);
    
}


#pragma endregion

#pragma region Physics

void StageCurtain::hit_test()
{
    size_t min_idx  = INT_MAX;
    float  min_dist = FLT_MAX;

    for(size_t idx = 0; idx < range_[0] * range_[1] ; idx += range_[1])
    {
        float dist = distance_from_point(quad_matrix_[idx], get_start_of_x() * get_sign() + points[0][0]);
        if(dist < min_dist)
        {
            min_idx  = idx;
            min_dist = dist;
        }
    }

    if(min_idx == INT_MAX)  return;
    if(min_dist > 0.1f)     return;

    float power = get_power() * 0.2f;
    if(is_right_) power *= get_sign();
    propagate(min_idx, power, decay_norm_distribute);
}

void StageCurtain::propagate(size_t index, float force, float(* decay_func)(int, float))
{
    // index보다 위쪽에만 전파
    // 위 쪽 거리는 옆 거리에 2배 가중치를 가짐
    
    glm::ivec2 self = {index / range_[1], index % range_[1]};
    for(size_t idx = 0 ; idx < index ; idx++)
    {
        glm::ivec2 target = {idx / range_[1], idx % range_[1]};
        int distance = abs(target[0] - self[0]) * 2 + abs(target[1] - self[1]);
        apply_force(idx, force * decay_func(distance, pow(range_[1], 2)));
    }
    
    solve_force();
    
}

void StageCurtain::apply_force(size_t index, float power)
{
    // index가 범위를 벗어난 경우 Pass
    if(index < 0 || index >= range_[0] * range_[1]) return;

    // index가 벽에 가장 가까운 곳을 가리킬 경우 Pass (끝은 고정)
    if(index % range_[1] == range_[1] - 1) return;
    
    points[index][0] += power;

    // 반대방향으로 Power가 작용한 경우 캔슬시킨다.
    if( (points[index][0] > 0 && get_sign() < 0) || (points[index][0] < 0 && get_sign() > 0)) points[index][0] = 0;
}

void StageCurtain::solve_force()
{
    // 상단 상태에 따른 Decay를 적용한다.
    decay_force();
    
    // 왼쪽에 있는 Vertex가 오른쪽의 Vertex를 넘어가지 못하도록 한다.
    for(size_t hidx = 0 ; hidx < range_[0] ; hidx++)
    {
        size_t sidx = range_[1] - 2;
        while(true)
        {
            size_t idx = get_matrix_idx(hidx, sidx);
            points[idx][0]  = glm::clamp(abs(points[idx][0]), 0.f, get_gap_of_x() + abs(points[idx+1][0]) - MIN_DISTANCE);
            points[idx][0] *= get_sign(); 

            // 가장 중앙에 가까운 Vertex까지 Solve했으면 다음 열로 넘어간다.
            if(sidx == 0) break;
            sidx--;
        }
    }
    
}

void StageCurtain::decay_force()
{
    
    // 위쪽 Point[idx][0] 값에 영향을 받아 Decay
    for(size_t sidx = 0 ; sidx < range_[1] ; sidx++)
    {
        for(size_t hidx = 1 ; hidx < range_[0] ; hidx++)
        {
            float up_additive = points[get_matrix_idx(hidx-1, sidx)][0];
            size_t self       = get_matrix_idx(hidx, sidx);
            points[self][0]  -= (points[self][0] - up_additive) * FORCE_DECAY;
        }
    }
}

#pragma endregion

void StageCurtain::open_curtain()
{
    is_opened_ = true;
}

void StageCurtain::close_curtain()
{
    is_opened_ = false;
}
