#pragma once

#include <Math.hpp>
#include <Vector3.hpp>

namespace Renderer {
class RenderableSphere {
public:
    Math::Vector3 center;
    Math::Scalar radius;
    
    Math::Vector3 color;
}
}
