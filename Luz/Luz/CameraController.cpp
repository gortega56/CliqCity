#include "stdafx.h"
#include "CameraController.h"
#include "Input.h"

using namespace gmath;

namespace Luz
{
    static const float g_radian = 3.1415926535f / 180.0f;

    CameraController::CameraController(std::shared_ptr<Input> pInput)
        : m_camera()
        , m_input(pInput)
        , m_movementSpeed(1.0f)
        , m_horizontalTurnSpeed(1.0f)
        , m_verticalTurnSpeed(1.0f)
        , m_mouseX(1.0f)
        , m_mouseY(1.0f)
    {

    }

    CameraController::CameraController()
        : m_camera()
        , m_input(nullptr)
        , m_movementSpeed(1.0f)
        , m_horizontalTurnSpeed(1.0f)
        , m_verticalTurnSpeed(1.0f)
        , m_mouseX(1.0f)
        , m_mouseY(1.0f)
    {

    }

    void CameraController::Update(double deltaSeconds)
    {
        if (m_input)
        {
            ScreenPoint mousePosition = m_input->mousePosition;
            float mouseX = (float)mousePosition.x;
            float mouseY = (float)mousePosition.y;

            Transform* pTransform = m_camera.GetTransform();

            if (m_input->GetMouseButton(MOUSEBUTTON_RIGHT))
            {
                euler rotation = pTransform->GetEuler();
                rotation.pitch += (mouseY - m_mouseY) * g_radian * m_verticalTurnSpeed;
                rotation.yaw += (mouseX - m_mouseX) * g_radian * m_horizontalTurnSpeed;
                rotation.roll = 0.0f;
                pTransform->SetRotation(rotation);
            }

            m_mouseX = mouseX;
            m_mouseY = mouseY;

            float3 position = pTransform->GetPosition();
            if (m_input->GetKey(KEYCODE_W))
            {
                position += pTransform->GetForward() * m_movementSpeed;
            }

            if (m_input->GetKey(KEYCODE_A))
            {
                position += -pTransform->GetRight() * m_movementSpeed;
            }

            if (m_input->GetKey(KEYCODE_S))
            {
                position += -pTransform->GetForward() * m_movementSpeed;
            }

            if (m_input->GetKey(KEYCODE_D))
            {
                position += pTransform->GetRight() * m_movementSpeed;
            }

            if (m_input->GetKey(KEYCODE_Q))
            {
                position += pTransform->GetUp() * m_movementSpeed;
            }

            if (m_input->GetKey(KEYCODE_E))
            {
                position += -pTransform->GetUp() * m_movementSpeed;
            }

            pTransform->SetPosition(position);
        }
    }
}