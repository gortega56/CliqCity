#include "stdafx.h"
#include "Camera.h"

using namespace gmath;

Camera::Camera()
    : m_projection(1.0f)
    , m_view(1.0f)
{

}

OrthographicCamera::OrthographicCamera(const float& width, const float& height, const float& nearPlane, const float& farPlane)
    : Camera()
    , m_width(width)
    , m_height(height)
    , m_near(nearPlane)
    , m_far(farPlane)
    , m_projectionDirty(true)
{
 
}

OrthographicCamera::OrthographicCamera()
    : Camera()
    , m_width(0.0f)
    , m_height(0.0f)
    , m_near(0.0f)
    , m_far(0.0f)
    , m_projectionDirty(true)
{

}

gmath::float4x4 OrthographicCamera::GetProjection()
{
    if (m_projectionDirty)
    {
        m_projection.p_data[0] = 2.0f / m_width;
        m_projection.p_data[1] = 0.0f;
        m_projection.p_data[2] = 0.0f;
        m_projection.p_data[3] = 0.0f;

        m_projection.p_data[4] = 0.0f;
        m_projection.p_data[5] = 2.0f / m_height;
        m_projection.p_data[6] = 0.0f;
        m_projection.p_data[7] = 0.0f;

        m_projection.p_data[ 8] = 0.0f;
        m_projection.p_data[ 9] = 0.0f;
        m_projection.p_data[10] = -2.0f / (m_far - m_near);
        m_projection.p_data[11] = 0.0f;

        m_projection.p_data[12] = 0.0f;
        m_projection.p_data[13] = 0.0f;
        m_projection.p_data[14] = 0.0f;
        m_projection.p_data[15] = 1.0f;
    }

    return m_projection;
}

void OrthographicCamera::SetWidth(const float& width)
{
    m_width = width;
    m_projectionDirty = true;
}

void OrthographicCamera::SetHeight(const float& height)
{
    m_height = height;
    m_projectionDirty = true;
}

void OrthographicCamera::SetNear(const float& nearPlane)
{
    m_near = nearPlane;
    m_projectionDirty = true;
}

void OrthographicCamera::SetFar(const float& farPlane)
{
    m_far = farPlane;
    m_projectionDirty = true;
}

PerspectiveCamera::PerspectiveCamera(const float& fov, const float& aspectRatio, const float& nearPlane, const float& farPlane)
    : Camera()
    , m_fov(fov)
    , m_aspectRatio(aspectRatio)
    , m_near(nearPlane)
    , m_far(farPlane)
    , m_projectionDirty(true)
{

}

PerspectiveCamera::PerspectiveCamera()
    : Camera()
    , m_fov(0.0f)
    , m_aspectRatio(0.0f)
    , m_near(0.0f)
    , m_far(0.0f)
    , m_projectionDirty(true)
{

}

inline gmath::float4x4 PerspectiveCamera::GetProjection()
{
    if (m_projectionDirty)
    {
#if _WIN32 || _WIN64
        m_projection = float4x4::normalized_perspective_lh(m_fov, m_aspectRatio, m_near, m_far);
#elif
        m_projection = float4x4::perspective_lh(m_fov, m_aspectRatio, m_near, m_far);
#endif
    }

    return m_projection;
}

void PerspectiveCamera::SetFieldOfView(const float& fov)
{
    m_fov = fov;
    m_projectionDirty = true;
}

void PerspectiveCamera::SetAspectRatio(const float& aspectRatio)
{
    m_aspectRatio = aspectRatio;
    m_projectionDirty = true;
}


void PerspectiveCamera::SetNear(const float& nearPlane)
{
    m_near = nearPlane;
    m_projectionDirty = true;
}

void PerspectiveCamera::SetFar(const float& farPlane)
{
    m_far = farPlane;
    m_projectionDirty = true;
}