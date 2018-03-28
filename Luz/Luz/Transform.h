#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H

class Transform
{
public:
    LUZ_API Transform();
    LUZ_API ~Transform();

    LUZ_API gmath::float4x3 GetWorld();
    LUZ_API gmath::float3x3 GetOrientation() const;

    LUZ_API gmath::quaternion GetRotation() const;
    LUZ_API gmath::euler GetEuler() const;
    LUZ_API gmath::float3 GetPosition() const;
    LUZ_API gmath::float3 GetScale() const;

    LUZ_API gmath::float3 GetRight() const;
    LUZ_API gmath::float3 GetUp() const;
    LUZ_API gmath::float3 GetForward() const;

    LUZ_API Transform* GetParent() const;

    LUZ_API void SetPosition(const gmath::float3& position);
    LUZ_API void SetRotation(const gmath::euler& euler);
    LUZ_API void SetRotation(const gmath::quaternion& quat);
    LUZ_API void SetScale(const gmath::float3& scale);

    LUZ_API void SetRotation(const float& pitch, const float& yaw, const float& roll);
    LUZ_API void SetRotation(const float& x, const float& y, const float& z, const float& w);
    LUZ_API void SetPosition(const float& x, const float& y, const float& z);
    LUZ_API void SetScale(const float& x, const float& y, const float& z);

    LUZ_API void MoveRight(const float& distance);
    LUZ_API void MoveUp(const float& distance);
    LUZ_API void MoveForward(const float& distance);

    LUZ_API void RotatePitch(const float& radians);
    LUZ_API void RotateYaw(const float& radians);
    LUZ_API void RotateRoll(const float& radians);

    LUZ_API bool IsDirty();

private:
    gmath::float4x3 m_world;
    gmath::float4x3 m_local;

    gmath::quaternion m_rotation;
    gmath::float3 m_position;
    gmath::float3 m_scale;

    gmath::float3 m_right;
    gmath::float3 m_up;
    gmath::float3 m_forward;

    Transform* m_parent;
    bool m_isDirty;
};

inline gmath::float3x3 Transform::GetOrientation() const
{
    return gmath::float3x3(m_world.p_rows[0], m_world.p_rows[1], m_world.p_rows[2]);
}

inline gmath::quaternion Transform::GetRotation() const
{
    return m_rotation;
}

inline gmath::euler Transform::GetEuler() const
{
    gmath::euler rollPitchYaw;
    m_rotation.to_euler(rollPitchYaw);
    return rollPitchYaw;
}

inline gmath::float3 Transform::GetPosition() const
{
    return m_position;
}

inline gmath::float3 Transform::GetScale() const
{
    return m_scale;
}

inline gmath::float3 Transform::GetRight() const
{
    return m_rotation * m_right;
}

inline gmath::float3 Transform::GetUp() const
{
    return m_rotation * m_up;
}

inline gmath::float3 Transform::GetForward() const
{
    return m_rotation * m_forward;
}

inline Transform* Transform::GetParent() const
{
    return m_parent;
}

#endif