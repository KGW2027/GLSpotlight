#pragma once
#include <vector>

#include "StageObject.h"


class StageRoom : public StageObject
{

    std::vector<Mesh> walls; 

protected:
    void pre_render() override;
    void rendering() override;
    void post_render() override;
public:
    
    void ready() override;
};
