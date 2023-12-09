#pragma once
#include "StageObject.h"
#include "../utils/GLSPhysics.h"

class StageCurtain : public StageObject, public GLSPhysics
{
    
    unsigned int tid_curtain_   ;
    bool         is_right_      ;
    bool         is_opened_     ;
    float        **points       ;
    float        curtain_start_ ;
    size_t       *range_        ;
    Material     mat_curtain_   ;

    vec3         *quad_matrix_  ;

#pragma region Draw
    size_t get_matrix_idx(const size_t row, const size_t col) { return row * range_[1] + col; }
    
    float  get_start_of_x() { return curtain_start_ * scale_[0];   }
    float  get_end_of_x()   { return 4.00f * scale_[0];            }
    float  get_gap_of_x()   { return (get_end_of_x() - get_start_of_x()) / static_cast<float>(range_[1]); }
    
    float  get_start_of_h() { return 2.00f * scale_[2];   }
    float  get_end_of_h()   { return -.55f * scale_[2];   }
    float  get_gap_of_h()   { return (get_start_of_h() - get_end_of_h()) / static_cast<float>(range_[0]); }
    
    float  get_sign() const { return is_right_ ? 1 : -1; }

    void apply_points();
    void draw_points();
#pragma  endregion
    
#pragma region Physics

    void hit_test();
    void propagate(size_t index, float force, float(*decay_func)(int, float));
    void apply_force(size_t index, float power);
    void solve_force();
    void decay_force();
    
#pragma endregion
    
protected:
    void pre_render() override;
    void rendering() override;
    void post_render() override;
public:
    
    void ready() override;
    void set_direction(bool is_right) { is_right_ = is_right; }

    void open_curtain ();
    void close_curtain();
};
