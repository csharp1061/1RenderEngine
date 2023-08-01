#pragma once

#include "../../core//math/math_headers.h"
#include <vector>
#include <memory>

namespace OEngine
{
	enum Camera_Movement
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	// 设置默认的canmera值
	const float YAW = -90.0f;
	const float PITCH = 0.0f;
	const float SPEED = 2.5f;
	const float SENSITIVITY = 0.1f;
	const float ZOOM = 45.0f;

	// 默认相机 -- FPS相机
	class CameraFPS
	{
	public:
		Vector3 m_position;
		Vector3 m_front;
		Vector3 m_up;
		Vector3 m_right;
		Vector3 m_worldUp;

		float m_yaw;
		float m_pitch;

		float m_movementSpeed;
		float m_mouseSensitivity;
		float m_zoom;

		CameraFPS(Vector3 position = Vector3::ZERO, Vector3 up = Vector3::UNIT_Y, float yaw = YAW, float pitch = PITCH) : m_front(Vector3::NEGATIVE_UNIT_Z), m_movementSpeed(SPEED), m_mouseSensitivity(SENSITIVITY), m_zoom(ZOOM)
		{
			m_position = position;
			m_worldUp = up;
			m_yaw = yaw;
			m_pitch = pitch;
			updateCameraVectors();
		}

		Matrix4x4 getViewMatrix()
		{
			return Math::makeLookAtMatrix(m_position, m_position + m_front, m_up);
		}

		void processKeyboard(Camera_Movement direction, float deltaTime)
		{
			float velocity = m_movementSpeed * deltaTime;
			if (direction == FORWARD)
				m_position += m_front * velocity;
			if (direction == BACKWARD)
				m_position -= m_front * velocity;
			if (direction == LEFT)
				m_position -= m_right * velocity;
			if (direction == RIGHT)
				m_position += m_right * velocity;
		}

		void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
		{
			xoffset *= m_mouseSensitivity;
			yoffset *= m_mouseSensitivity;

			m_yaw += xoffset;
			m_pitch += yoffset;

			if (constrainPitch)
			{
				if (m_pitch > 89.0f)
					m_pitch = 89.0f;
				if (m_pitch < -89.0f)
					m_pitch = -89.0f;
			}

			updateCameraVectors();
		}

		void processMouseScroll(float yoffset)
		{
			m_zoom -= (float)yoffset;
			if (m_zoom < 1.0f)
				m_zoom = 1.0f;
			if (m_zoom > 45.0f)
				m_zoom = 45.0f;
		}

	private:
		// 更新相机各个位置方向
		void updateCameraVectors()
		{
			Vector3 front;
			front.x = Math::cos(Radian(m_yaw)) * Math::cos(Radian(m_pitch));
			front.y = Math::sin(Radian(m_pitch));
			front.z = Math::sin(Radian(m_yaw)) * Math::cos(Radian(m_pitch));
			m_front = front.normalizedCopy();
			m_right = front.crossProduct(m_worldUp).normalizedCopy();
			m_up = m_right.crossProduct(m_front).normalizedCopy();
		}
	};

	class Camera
	{
	public:
		typedef std::shared_ptr<Camera> Ptr;

		Vector3 m_eye;
		Vector3 m_target;
		Vector3 m_up;

		Vector3 x;
		Vector3 y;
		Vector3 z;

		float aspect;

		Camera(Vector3 e, Vector3 t, Vector3 up, float a) : m_eye(e), m_target(t), m_up(up), aspect(a) {}
		
		~Camera() = default;
	};

	void update_camera_pos(Camera::Ptr camera);

	void handle_mouse_events(Camera::Ptr camera);

	void handle_key_events(Camera::Ptr camera);

	void handle_events(Camera::Ptr camera);
} // OEngine