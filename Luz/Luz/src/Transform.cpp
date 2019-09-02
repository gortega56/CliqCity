#include "stdafx.h"
#include "Transform.h"

using namespace lina;

Transform::Transform()
    : m_world(float4x3::identity())
    , m_local(float4x3::identity())
    , m_rotation(0.0f, 0.0f, 0.0f, 1.0f)
    , m_position(0.0f, 0.0f, 0.0f)
    , m_scale(1.0f, 1.0f, 1.0f)
    , m_right(1.0f, 0.0f, 0.0f)
    , m_up(0.0f, 1.0f, 0.0f)
    , m_forward(0.0f, 0.0f, 1.0f)
    , m_parent(nullptr)
    , m_isDirty(true)
{

}

Transform::~Transform()
{

}

float4x3 Transform::GetWorld() 
{
    if (IsDirty())
    {
        m_local = float4x3::scale(m_scale) * m_rotation.matrix4x3() * float4x3::translate(m_position);
        m_world = (m_parent == nullptr) ? m_local : m_local * m_parent->GetWorld();
        m_isDirty = false;
    }

    return m_world;
}

void Transform::SetPosition(const float3& position)
{
    m_position = position;
    m_isDirty = true;
}

void Transform::SetRotation(const float3& euler)
{
    m_rotation = quaternion::create(euler.x, euler.y, euler.z);
    m_isDirty = true;
}

void Transform::SetRotation(const quat& quat)
{
    m_rotation = quat;
    m_isDirty = true;
}

void Transform::SetRotation(const float3x3& mat)
{
    m_rotation = quaternion::create(mat);
    m_isDirty = true;
}

void Transform::SetScale(const float3& scale)
{
    m_scale = scale;
    m_isDirty = true;
}

void Transform::SetRotation(const float& pitch, const float& yaw, const float& roll)
{
    m_rotation = quaternion::create(pitch, yaw, roll);
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
    m_rotation *= quaternion::create(radians, 0.0f, 0.0f);
    m_isDirty = true;
}

void Transform::RotateYaw(const float& radians)
{
    m_rotation *= quaternion::create(0.0f, radians, 0.0f);
    m_isDirty = true;
}

void Transform::RotateRoll(const float& radians)
{
    m_rotation *= quaternion::create(0.0f, 0.0f, radians);
    m_isDirty = true;
}

void Transform::LookAt(const float3& target)
{
    /*float3 f = normalize(float3(target - m_position, 0.0f));
    float3 s = normalize(cross(m_up, f));
    float3 u = cross(f, s);
*/
}

void Transform::LookAt(const float x, const float y, const float z)
{

}

void Transform::LookTo(const float3& direction)
{

}

void Transform::LookTo(const float x, const float y, const float z)
{

}

bool Transform::IsDirty()
{
    m_isDirty |= (m_parent != nullptr && m_parent->IsDirty());
    return m_isDirty;
}
