#pragma once
#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#ifndef CAMERA_H
#include "Camera.h"
#endif


namespace Luz
{
    class Input;

    class CameraController
    {
    public:
        LUZ_API CameraController(std::shared_ptr<Input> pInput);
        LUZ_API CameraController();
        LUZ_API ~CameraController() = default;

        LUZ_API void Update(double deltaSeconds);

        LUZ_API PerspectiveCamera* GetCamera();
        LUZ_API float GetMovementSpeed() const;
        LUZ_API float GetHorizontalTurnSpeed() const;
        LUZ_API float GetVerticalTurnSpeed() const;

    private:
        PerspectiveCamera m_camera;
        std::shared_ptr<Input> m_input;

        float m_movementSpeed;
        float m_horizontalTurnSpeed;
        float m_verticalTurnSpeed;
        float m_mouseX;
        float m_mouseY;
    };

    inline PerspectiveCamera* CameraController::GetCamera()
    {
        return &m_camera;
    }

    inline float CameraController::GetMovementSpeed() const
    {
        return m_movementSpeed;
    }

    inline float CameraController::GetHorizontalTurnSpeed() const
    {
        return m_horizontalTurnSpeed;
    }

    inline float CameraController::GetVerticalTurnSpeed() const
    {
        return m_verticalTurnSpeed;
    }
}
#endif