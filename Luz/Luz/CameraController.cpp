#include "stdafx.h"
#include "CameraController.h"
#include "Platform.h"
#include "Input.h"

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

        float moveSpeed = std::clamp(m_movementSpeed + Platform::GetMouseWheelDelta(), 0.001f, 1000.0f);
        m_movementSpeed = moveSpeed;
       
        float3 prev = pTransform->GetPosition();
		float3 current = prev;

        if (Platform::GetKey(Platform::KEYCODE_W))
        {
			current += pTransform->GetForward() * m_movementSpeed * dt;
        }

        if (Platform::GetKey(Platform::KEYCODE_A))
        {
			current += -pTransform->GetRight() * m_movementSpeed * dt;
        }

        if (Platform::GetKey(Platform::KEYCODE_S))
        {
			current += -pTransform->GetForward() * m_movementSpeed * dt;
        }

        if (Platform::GetKey(Platform::KEYCODE_D))
        {
			current += pTransform->GetRight() * m_movementSpeed * dt;
        }

        if (Platform::GetKey(Platform::KEYCODE_Q))
        {
			current += pTransform->GetUp() * m_movementSpeed * dt;
        }

        if (Platform::GetKey(Platform::KEYCODE_E))
        {
			current += -pTransform->GetUp() * m_movementSpeed * dt;
        }

		float3 interp = lerp(prev, current, 0.75f);

        pTransform->SetPosition(interp);
    }
}