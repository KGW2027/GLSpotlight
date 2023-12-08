#pragma once

#include "StageObject.h"


class StageRoom : public StageObject
{

protected:
    void pre_render() override;
    void rendering() override;
    void post_render() override;
public:
    
    void ready() override;
};
