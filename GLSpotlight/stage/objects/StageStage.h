#pragma once
#include "StageObject.h"

class StageStage : public StageObject
{

    unsigned int tid_stage;
    
protected:
    void pre_render() override;
    void rendering() override;
    void post_render() override;
public:
    
    void ready() override;
    
};
