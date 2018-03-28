#include "stdafx.h"
#include "Transform.h"

using namespace gmath;

Transform::Transform()
    : m_world(1.0f)
    , m_local(1.0f)
    , m_rotation(0.0f, 0.0f, 0.0f, 1.0f)
    , m_position(0.0f, 0.0f, 0.0f)
    , m_scale(1.0f, 1.0f, 1.0f)
    , m_right(1.0f, 0.0f, 0.0f)
    , m_up(0.0f, 1.0f, 0.0f)
    , m_forward(0.0f, 0.0f, 1.0f)
    , m_parent(nullptr)
    , m_isDirty(false)
{

}

Transform::~Transform()
{

}

float4x3 Transform::GetWorld() 
{
    if (IsDirty())
    {
        float4x3 orientation;
        m_rotation.to_matrix(orientation);

        m_local = float4x3::scale(m_scale) * orientation * float4x3::translate(m_position);
        m_world = (m_parent == nullptr) ? m_local : m_local * m_parent->GetWorld();
    }

    return m_world;
}

void Transform::SetPosition(const float3& position)
{
    m_position = position;
    m_isDirty = true;
}

void Transform::SetRotation(const euler& euler)
{
    m_rotation = quaternion(euler);
    m_isDirty = true;
}

void Transform::SetRotation(const quaternion& quat)
{
    m_rotation = quat;
    m_isDirty = true;
}

void Transform::SetScale(const float3& scale)
{
    m_scale = scale;
    m_isDirty = true;
}

void Transform::SetRotation(const float& pitch, const float& yaw, const float& roll)
{
    euler rollPitchYaw = euler(pitch, yaw, roll);
    m_rotation = quaternion(rollPitchYaw);
    m_isDirty = true;
}

void Transform::SetRotation(const float& x, const float& y, const float& z, const float& w)
{
    m_rotation.x = x;
    m_rotation.y = y;
    m_rotation.z = z;
    m_rotation.w = w;
    m_isDirty = true;

}

void Transform::SetPosition(const float& x, const float& y, const float& z)
{
    m_position.x = x;
    m_position.y = y;
    m_position.z = z;
    m_isDirty = true;

}

void Transform::SetScale(const float& x, const float& y, const float& z)
{
    m_scale.x = x;
    m_scale.y = y;
    m_scale.z = z;
    m_isDirty = true;
}

void Transform::MoveRight(const float& distance)
{
    m_position += m_right * distance;
    m_isDirty = true;
}

void Transform::MoveUp(const float& distance)
{
    m_position += m_up * distance;
    m_isDirty = true;
}

void Transform::MoveForward(const float& distance)
{
    m_position += m_forward * distance;
    m_isDirty = true;
}

void Transform::RotatePitch(const float& radians)
{
    euler e = euler(radians, 0.0f, 0.0f);
    m_rotation *= quaternion(e);
    m_isDirty = true;
}

void Transform::RotateYaw(const float& radians)
{
    euler e = euler(0.0f, radians, 0.0f);
    m_rotation *= quaternion(e);
    m_isDirty = true;
}

void Transform::RotateRoll(const float& radians)
{
    euler e = euler(0.0f, 0.0f, radians);
    m_rotation *= quaternion(e);
    m_isDirty = true;
}

bool Transform::IsDirty()
{
    m_isDirty |= (m_parent != nullptr && m_parent->IsDirty());
    return m_isDirty;
}
