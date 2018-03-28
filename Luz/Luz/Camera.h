#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#ifndef TRANSFORM_H
#include "Transform.h"
#endif

class Camera
{
public:
    LUZ_API Camera();
    LUZ_API ~Camera() = default;

    LUZ_API gmath::float4x4 GetProjection();
    LUZ_API gmath::float4x4 GetView();

    LUZ_API Transform* GetTransform();

protected:
    gmath::float4x4 m_projection;
    gmath::float4x4 m_view;

    Transform m_transform;
};

class OrthographicCamera : public Camera
{
public:
    LUZ_API OrthographicCamera(const float& width, const float& height, const float& nearPlane, const float& farPlane);
    LUZ_API OrthographicCamera();
    LUZ_API ~OrthographicCamera() = default;

    LUZ_API gmath::float4x4 GetProjection();

    LUZ_API float GetWidth() const;
    LUZ_API float GetHeight() const;
    LUZ_API float GetNear() const;
    LUZ_API float GetFar() const;

    LUZ_API void SetWidth(const float& width);
    LUZ_API void SetHeight(const float& height);
    LUZ_API void SetNear(const float& nearPlane);
    LUZ_API void SetFar(const float& farPlane);

private:
    float m_width;
    float m_height;
    float m_near;
    float m_far;

    bool m_projectionDirty;
};

class PerspectiveCamera : public Camera
{
public:
    LUZ_API PerspectiveCamera(const float& fov, const float& aspectRatio, const float& nearPlane, const float& farPlane);
    LUZ_API PerspectiveCamera();
    LUZ_API ~PerspectiveCamera() = default;

    LUZ_API gmath::float4x4 GetProjection();

    LUZ_API float GetFieldOfView() const;
    LUZ_API float GetAspectRatio() const;
    LUZ_API float GetNear() const;
    LUZ_API float GetFar() const;

    LUZ_API void SetFieldOfView(const float& fov);
    LUZ_API void SetAspectRatio(const float& aspectRatio);
    LUZ_API void SetNear(const float& nearPlane);
    LUZ_API void SetFar(const float& farPlane);

private:
    float m_fov;
    float m_aspectRatio;
    float m_near;
    float m_far;

    bool m_projectionDirty;
};

inline gmath::float4x4 Camera::GetProjection()
{
    return m_projection;
}

inline gmath::float4x4 Camera::GetView()
{
    if (m_transform.IsDirty())
    {
        m_view = gmath::float4x4::look_to_lh(m_transform.GetForward(), m_transform.GetPosition(), m_transform.GetUp());
    }

    return m_view;
}

Transform* Camera::GetTransform()
{
    return &m_transform;
}

inline float OrthographicCamera::GetWidth() const
{
    return m_width;
}

inline float OrthographicCamera::GetHeight() const
{
    return m_height;
}

inline float OrthographicCamera::GetNear() const
{
    return m_near;
}

inline float OrthographicCamera::GetFar() const
{
    return m_far;
}

inline float PerspectiveCamera::GetFieldOfView() const
{
    return m_fov;
}

inline float PerspectiveCamera::GetAspectRatio() const
{
    return m_aspectRatio;
}

inline float PerspectiveCamera::GetNear() const
{
    return m_near;
}

inline float PerspectiveCamera::GetFar() const
{
    return m_far;
}

#endif