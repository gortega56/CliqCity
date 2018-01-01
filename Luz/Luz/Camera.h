#pragma once
#ifndef CAMERA_H
#define CAMERA_H

class Camera
{
public:
    Camera();
    ~Camera();

    inline const gmath::float4x4& Projection() const { return m_projection; }
    inline const gmath::float4x4& View() const { return m_view; }

    gmath::float3 Forward() const;

private:
    gmath::float4x4 m_projection;
    gmath::float4x4 m_view;
};

#endif