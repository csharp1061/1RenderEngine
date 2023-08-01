#include "./camera.h"
#include "../platform/win32.h"

namespace OEngine
{
	void update_camera_pos(Camera::Ptr camera)
	{
		Vector3 from_target = camera->m_eye - camera->m_target;
		float radius = from_target.length();

		float phi = (float)atan2(from_target[0], from_target[2]); // 水平角
		float theta = (float)acos(from_target[1] / radius); // 垂直旋转部分

		float x_delta = window->mouse_info.orbit_delta[0] / window->width;
		float y_delta = window->mouse_info.orbit_delta[1] / window->height;

		radius *= (float)pow(0.95, window->mouse_info.wheel_delta);

		// 常数因子 控制相机移动速度
		float factor = 1.5f * Math_PI;

		phi += x_delta * factor;
		theta += y_delta * factor;

		if (theta > Math_PI)	theta = Math_PI - Float_EPSILON * 100;
		if (theta < 0)			theta = Float_EPSILON * 100;

		camera->m_eye[0] = camera->m_target[0] + radius * sin(phi) * sin(theta);
		camera->m_eye[1] = camera->m_target[1] + radius * cos(theta);
		camera->m_eye[2] = camera->m_target[2] + radius * sin(theta) * cos(phi);

		factor = radius * (float)tan(60.0 / 360 * Math_PI) * 2.2;
		x_delta = window->mouse_info.fv_delta[0] / window->width;
		y_delta = window->mouse_info.fv_delta[1] / window->height;
		Vector3 left = x_delta * factor * camera->x;
		Vector3 up	 = y_delta * factor * camera->y;

		camera->m_eye += (left - up);
		camera->m_target += (left - up);
	}

	void handle_mouse_events(Camera::Ptr camera)
	{
		if (window->buttons[0])
		{
			Vector2 cur_pos = get_mouse_pos();
			window->mouse_info.orbit_delta = window->mouse_info.orbit_delta - cur_pos;
			window->mouse_info.orbit_pos = cur_pos;
		}

		if (window->buttons[1])
		{
			Vector2 cur_pos = get_mouse_pos();
			window->mouse_info.fv_delta = window->mouse_info.fv_pos - cur_pos;
			window->mouse_info.fv_pos = cur_pos;
		}

		update_camera_pos(camera);
	}

	void handle_key_events(Camera::Ptr camera)
	{
		float distance = (camera->m_target - camera->m_eye).length();

		if (window->keys['W'])
		{
			camera->m_eye += -10.0 / window->width * camera->z * distance;
		}
		if (window->keys['S'])
		{
			camera->m_eye += 0.05f * camera->z;
		}
		if (window->keys[VK_UP] || window->keys['Q'])
		{
			camera->m_eye += 0.05f * camera->y;
			camera->m_target += 0.05f * camera->y;
		}
		if (window->keys[VK_DOWN] || window->keys['E'])
		{
			camera->m_eye += -0.05f * camera->y;
			camera->m_target += -0.05f * camera->y;
		}
		if (window->keys[VK_LEFT] || window->keys['A'])
		{
			camera->m_eye += -0.05f * camera->x;
			camera->m_target += -0.05f * camera->x;
		}
		if (window->keys[VK_RIGHT] || window->keys['D'])
		{
			camera->m_eye += 0.05f * camera->x;
			camera->m_target += 0.05f * camera->x;
		}
		if (window->keys[VK_ESCAPE])
		{
			window->is_close = 1;
		}
	}

	void handle_events(Camera::Ptr camera)
	{
		camera->z = (camera->m_eye - camera->m_target).normalizedCopy();
		camera->x = camera->m_up.crossProduct(camera->z).normalizedCopy();
		camera->y = camera->z.crossProduct(camera->x).normalizedCopy();

		handle_mouse_events(camera);
		handle_key_events(camera);
	}
} // OEngine