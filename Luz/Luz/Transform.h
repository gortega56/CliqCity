#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H

class Transform
{
public:
    LUZ_API Transform();
    LUZ_API ~Transform();

    LUZ_API float4x3 GetWorld();
    LUZ_API float3x3 GetOrientation() const;

    LUZ_API quat GetRotation() const;
    LUZ_API float3 GetEuler() const;
    LUZ_API float3 GetPosition() const;
    LUZ_API float3 GetScale() const;

    LUZ_API float3 GetRight() const;
    LUZ_API float3 GetUp() const;
    LUZ_API float3 GetForward() const;

    LUZ_API Transform* GetParent() const;

    LUZ_API void SetPosition(const float3& position);
    LUZ_API void SetRotation(const float3& euler);
    LUZ_API void SetRotation(const quat& quat);
    LUZ_API void SetRotation(const float3x3& mat);
    LUZ_API void SetScale(const float3& scale);

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

    LUZ_API void LookAt(const float3& position);
    LUZ_API void LookAt(const float x, const float y, const float z);

    LUZ_API void LookTo(const float3& direction);
    LUZ_API void LookTo(const float x, const float y, const float z);

    LUZ_API bool IsDirty();

private:
    float4x3 m_world;
    float4x3 m_local;

    quat m_rotation;
    float3 m_position;
    float3 m_scale;

    float3 m_right;
    float3 m_up;
    float3 m_forward;

    Transform* m_parent;
    bool m_isDirty;
};

inline float3x3 Transform::GetOrientation() const
{
    return float3x3(m_world.p_rows[0], m_world.p_rows[1], m_world.p_rows[2]);
}

inline quat Transform::GetRotation() const
{
    return m_rotation;
}

inline float3 Transform::GetEuler() const
{
    return m_rotation.euler();
}

inline float3 Transform::GetPosition() const
{
    return m_position;
}

inline float3 Transform::GetScale() const
{
    return m_scale;
}

inline float3 Transform::GetRight() const
{
    return m_rotation * m_right;
}

inline float3 Transform::GetUp() const
{
    return m_rotation * m_up;
}

inline float3 Transform::GetForward() const
{
    return m_rotation * m_forward;
}

inline Transform* Transform::GetParent() const
{
    return m_parent;
}

#endif