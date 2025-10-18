#pragma once

#include "math.h"

namespace ana
{
class Camera
{
public:
    void setOrthographicProjection(float left, float right, float top, float botton, float near, float far);
    void setPerspectiveProjection(float fov, float aspect, float zNear, float zFar);
};

} // namespace ana