#include "stdafx.h"
#include "Camera.h"

using namespace gmath;

Camera::Camera()
{

}

Camera::~Camera()
{

}

float3 Camera::Forward() const
{
    return { m_view.p_data[0], m_view.p_data[0], m_view.p_data[0] };
}