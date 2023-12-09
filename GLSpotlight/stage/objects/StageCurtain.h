#pragma once
#include "StageObject.h"

class StageCurtain : public StageObject
{
    
    unsigned int tid_curtain_   ;
    bool         is_right_      ;
    float        **points       ;
    size_t       *range_        ;

    vec3         *quad_matrix_  ;

    size_t get_matrix_idx(const size_t row, const size_t col) { return row * range_[1] + col; }
    
    float  get_start_of_x() { return 1.50f * scale_[0];   }
    float  get_end_of_x()   { return 4.00f * scale_[0];   }
    float  get_gap_of_x()   { return (get_end_of_x() - get_start_of_x()) / static_cast<float>(range_[1]); }
    
    float  get_start_of_h() { return 2.00f * scale_[2];   }
    float  get_end_of_h()   { return -.55f * scale_[2];   }
    float  get_gap_of_h()   { return (get_start_of_h() - get_end_of_h()) / static_cast<float>(range_[0]); }
    
    float  get_sign() const { return is_right_ ? 1 : -1; }

    void apply_points();
    void draw_points();
    
protected:
    void pre_render() override;
    void rendering() override;
    void post_render() override;
public:
    
    void ready() override;
    void set_direction(bool is_right) { is_right_ = is_right; }
};
