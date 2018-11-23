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
        , m_movementSpeed(1.0f)
        , m_horizontalTurnSpeed(1.0f)
        , m_verticalTurnSpeed(1.0f)
        , m_mouseX(1.0f)
        , m_mouseY(1.0f)
    {

    }

    void CameraController::Update(double deltaSeconds)
    {
        Platform::ScreenPoint mousePosition = Platform::GetMousePosition();
        float mouseX = (float)mousePosition.x;
        float mouseY = (float)mousePosition.y;

        Transform* pTransform = m_camera.GetTransform();

        if (Platform::GetMouseButton(Platform::MOUSEBUTTON_RIGHT))
        {
            float3 rotation = pTransform->GetEuler();
            rotation.x += (mouseY - m_mouseY) * g_radian * m_verticalTurnSpeed;
            rotation.y += (mouseX - m_mouseX) * g_radian * m_horizontalTurnSpeed;
            rotation.z = 0.0f;
            pTransform->SetRotation(rotation);
        }

        m_mouseX = mouseX;
        m_mouseY = mouseY;

        float3 position = pTransform->GetPosition();
        if (Platform::GetKey(Platform::KEYCODE_W))
        {
            position += pTransform->GetForward() * m_movementSpeed;
        }

        if (Platform::GetKey(Platform::KEYCODE_A))
        {
            position += -pTransform->GetRight() * m_movementSpeed;
        }

        if (Platform::GetKey(Platform::KEYCODE_S))
        {
            position += -pTransform->GetForward() * m_movementSpeed;
        }

        if (Platform::GetKey(Platform::KEYCODE_D))
        {
            position += pTransform->GetRight() * m_movementSpeed;
        }

        if (Platform::GetKey(Platform::KEYCODE_Q))
        {
            position += pTransform->GetUp() * m_movementSpeed;
        }

        if (Platform::GetKey(Platform::KEYCODE_E))
        {
            position += -pTransform->GetUp() * m_movementSpeed;
        }

        pTransform->SetPosition(position);
    }
}