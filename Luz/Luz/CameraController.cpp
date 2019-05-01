#include "stdafx.h"
#include "CameraController.h"
#include "Platform.h"
#include "PlatformInput.h"
#include "PlatformWindow.h"

using namespace lina;

namespace Luz
{
    static const float g_radian = 3.1415926535f / 180.0f;

    CameraController::CameraController()
        : m_camera()
        , m_movementSpeed(50.0f)
        , m_horizontalTurnSpeed(50.0f)
        , m_verticalTurnSpeed(50.0f)
        , m_mouseX(1.0f)
        , m_mouseY(1.0f)
    {

    }

    void CameraController::Update(double deltaSeconds)
    {
		float dt = static_cast<float>(deltaSeconds);

        Platform::ScreenPoint mousePosition = Platform::GetMousePosition();
        float mouseX = (float)mousePosition.x;
        float mouseY = (float)mousePosition.y;

        Transform* pTransform = m_camera.GetTransform();

        if (Platform::GetMouseButton(Platform::MOUSEBUTTON_RIGHT))
        {
			float3 prev = pTransform->GetEuler();
			prev.z = 0.0f;

			float3 current = prev;
			current.x += (mouseY - m_mouseY) * g_radian * m_verticalTurnSpeed * dt;
			current.y += (mouseX - m_mouseX) * g_radian * m_horizontalTurnSpeed * dt;
			current.z = 0.0f;

			quat q0 = quaternion::create(prev.x, prev.y, prev.z);
			quat q1 = quaternion::create(current.x, current.y, current.z);
		
			quat interp = slerp(q0, q1, 0.35f);
			interp = normalize(interp);

            pTransform->SetRotation(interp);
        }

        m_mouseX = mouseX;
        m_mouseY = mouseY;

		static float s_mouseWheelDelta = 0.0f;
		static float s_increments = 10.0f;
		static float s_min_speed = 0.001f;
		static float s_max_speed = 500.0f;

		s_mouseWheelDelta = std::clamp(s_mouseWheelDelta + Platform::GetMouseWheelDelta(), 0.0f, s_increments);

		float t = (s_mouseWheelDelta / s_increments);
		m_movementSpeed = (1.0f - t) * s_min_speed + s_max_speed * t;;
       
        float3 prev = pTransform->GetPosition();
		float3 current = prev;

        if (Platform::GetKey(Platform::KEYTYPE_W))
        {
			current += pTransform->GetForward() * m_movementSpeed * dt;
        }

        if (Platform::GetKey(Platform::KEYTYPE_A))
        {
			current += -pTransform->GetRight() * m_movementSpeed * dt;
        }

        if (Platform::GetKey(Platform::KEYTYPE_S))
        {
			current += -pTransform->GetForward() * m_movementSpeed * dt;
        }

        if (Platform::GetKey(Platform::KEYTYPE_D))
        {
			current += pTransform->GetRight() * m_movementSpeed * dt;
        }

        if (Platform::GetKey(Platform::KEYTYPE_Q))
        {
			current += pTransform->GetUp() * m_movementSpeed * dt;
        }

        if (Platform::GetKey(Platform::KEYTYPE_E))
        {
			current += -pTransform->GetUp() * m_movementSpeed * dt;
        }

		float3 interp = lerp(prev, current, 0.75f);

        pTransform->SetPosition(interp);
    }
}