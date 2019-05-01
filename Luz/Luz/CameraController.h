#pragma once
#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#ifndef CAMERA_H
#include "Camera.h"
#endif

namespace Luz
{
    class CameraController
    {
    public:
        LUZ_API CameraController();
        
        LUZ_API ~CameraController() = default;

        LUZ_API void Update(double deltaSeconds);

        LUZ_API PerspectiveCamera* GetCamera();
        
        LUZ_API float GetMovementSpeed() const;
        
        LUZ_API float GetHorizontalTurnSpeed() const;
        
        LUZ_API float GetVerticalTurnSpeed() const;

        LUZ_API void SetMovementSpeed(const float speed);

        LUZ_API void SetHorizontalTurnSpeed(const float speed);

        LUZ_API void SetVerticalTurnSpeed(const float speed);

		LUZ_API void SetWindowWidth(const unsigned int width);

		LUZ_API void SetWindowHeight(const unsigned int height);

    private:
        PerspectiveCamera m_camera;

        float m_movementSpeed;
        float m_horizontalTurnSpeed;
        float m_verticalTurnSpeed;
        float m_mouseX;
        float m_mouseY;
		unsigned short m_windowWidth;
		unsigned short m_windowHeight;
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

    inline void CameraController::SetMovementSpeed(const float speed)
    {
        m_movementSpeed = speed;
    }

    inline void CameraController::SetHorizontalTurnSpeed(const float speed)
    {
        m_horizontalTurnSpeed = speed;
    }

    inline void CameraController::SetVerticalTurnSpeed(const float speed)
    {
        m_verticalTurnSpeed = speed;
    }

	inline void CameraController::SetWindowWidth(const unsigned int width)
	{
		m_windowWidth = static_cast<unsigned short>(width);
	}

	inline void CameraController::SetWindowHeight(const unsigned int height)
	{
		m_windowHeight = static_cast<unsigned short>(height);
	}
}
#endif