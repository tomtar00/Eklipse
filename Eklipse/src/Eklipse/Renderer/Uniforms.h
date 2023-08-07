#pragma once
#include <glm/glm.hpp>

namespace Eklipse
{
    struct ModelViewProjUBO 
    {
        glm::mat4 mvp;
    };
    struct ParticleUBO
    {
        float deltaTime;
    };
}