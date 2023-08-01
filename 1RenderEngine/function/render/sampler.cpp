#include "./sampler.h"

#include <stdlib.h>
#include <thread>

namespace OEngine
{
	static int cal_cubemap_uv(Vector3 direction, Vector2& uv)
	{
		int face_index = -1;
		float ma = 0, sc = 0, tc = 0;
		float abs_x = fabs(direction[0]), abs_y = fabs(direction[1]), abs_z = fabs(direction[2]);

		// 首先根据主要的轴确定主要方向
		// 面的编号从0到5，分别表示右侧面、左侧面、顶部面、底部面、前面和后面
		if (abs_x > abs_y && abs_x > abs_z)			/* major axis -> x */
		{
			ma = abs_x;
			if (direction.x > 0)					/* positive x */
			{
				face_index = 0;
				sc = +direction.z;
				tc = +direction.y;
			}
			else									/* negative x */
			{
				face_index = 1;
				sc = -direction.z;
				tc = +direction.y;
			}
		}
		else if (abs_y > abs_z)						/* major axis -> y */
		{
			ma = abs_y;
			if (direction.y > 0)					/* positive y */
			{
				face_index = 2;
				sc = +direction.x;
				tc = +direction.z;
			}
			else									/* negative y */
			{
				face_index = 3;
				sc = +direction.x;
				tc = -direction.z;
			}
		}
		else										/* major axis -> z */
		{
			ma = abs_z;
			if (direction.z > 0)					/* positive z */
			{
				face_index = 4;
				sc = -direction.x;
				tc = +direction.y;
			}
			else									/* negative z */
			{
				face_index = 5;
				sc = +direction.x;
				tc = +direction.y;
			}
		}

		// 根据坐标分量计算uv的值
		uv[0] = (sc / ma + 1.0f) / 2.0f;
		uv[1] = (tc / ma + 1.0f) / 2.0f;

		// 返回面得编号
		return face_index;
	}

	// 颜色采样
	Vector3 texture_sample(Vector2 uv, TGAImage* image)
	{
		uv[0] = fmod(uv[0], 1);
		uv[1] = fmod(uv[1], 1);
		//printf("%f %f\n", uv[0], uv[1]);
		int uv0 = uv[0] * image->get_width();
		int uv1 = uv[1] * image->get_height();
		TGAColor c = image->get(uv0, uv1);
		Vector3 res;
		for (int i = 0; i < 3; i++)
			res[2 - i] = (float)c[i] / 255.f;
		return res;
	}

	Vector3 cubemap_sample(Vector3 direction, cubemap_t* cubemap)
	{
		Vector3 color;
		Vector2 uv;
		int index = cal_cubemap_uv(direction, uv);

		color = texture_sample(uv, cubemap->faces[index]);
		return color;
	}

	/* for image-based lighting pre-computing */
	float radicalInverse_VdC(unsigned int bits) {
		bits = (bits << 16u) | (bits >> 16u);
		bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
		bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
		bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
		bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
		return float(bits) * 2.3283064365386963e-10; // / 0x100000000
	}

	Vector2 hammersley2d(unsigned int i, unsigned int N) {
		return Vector2(float(i) / float(N), radicalInverse_VdC(i));
	}

	Vector3 hemisphereSample_uniform(float u, float v) {
		float phi = v * 2.0f * Math_PI;
		float cosTheta = 1.0f - u;
		float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
		return Vector3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
	}

	Vector3 hemisphereSample_cos(float u, float v) {
		float phi = v * 2.0 * Math_PI;
		float cosTheta = sqrt(1.0 - u);
		float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
		return Vector3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
	}

	Vector3 ImportanceSampleGGX(Vector2 Xi, Vector3 N, float roughness)
	{
		float a = roughness * roughness;

		float phi = 2.0 * Math_PI * Xi.x;
		float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
		float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

		// from spherical coordinates to cartesian coordinates
		Vector3 H;
		H[0] = cos(phi) * sinTheta;
		H[1] = sin(phi) * sinTheta;
		H[2] = cosTheta;

		// from tangent-space vector to world-space sample vector
		Vector3 up = abs(N.z) < 0.999 ? Vector3(0.0, 0.0, 1.0) : Vector3(1.0, 0.0, 0.0);
		Vector3 tangent = up.crossProduct(N).normalizedCopy();
		Vector3 bitangent = N.crossProduct(tangent);

		Vector3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
		return sampleVec.normalizedCopy();
	}

	static float SchlickGGX_geometry(float n_dot_v, float roughness)
	{
		float r = (1 + roughness);
		float k = r * r / 8.0;
		k = roughness * roughness / 2.0f;
		return n_dot_v / (n_dot_v * (1 - k) + k);
	}

	static float geometry_Smith(float n_dot_v, float n_dot_l, float roughness)
	{
		float g1 = SchlickGGX_geometry(n_dot_v, roughness);
		float g2 = SchlickGGX_geometry(n_dot_l, roughness);

		return g1 * g2;
	}

	void set_normal_coord(int face_id, int x, int y, float& x_coord, float& y_coord, float& z_coord, float length = 255)
	{
		switch (face_id)
		{
		case 0:   //positive x (right face)
			x_coord = 0.5f;
			y_coord = -0.5f + y / length;
			z_coord = -0.5f + x / length;
			break;
		case 1:   //negative x (left face)		
			x_coord = -0.5f;
			y_coord = -0.5f + y / length;
			z_coord = 0.5f - x / length;
			break;
		case 2:   //positive y (top face)
			x_coord = -0.5f + x / length;
			y_coord = 0.5f;
			z_coord = -0.5f + y / length;
			break;
		case 3:   //negative y (bottom face)
			x_coord = -0.5f + x / length;
			y_coord = -0.5f;
			z_coord = 0.5f - y / length;
			break;
		case 4:   //positive z (back face)
			x_coord = 0.5f - x / length;
			y_coord = -0.5f + y / length;
			z_coord = 0.5f;
			break;
		case 5:   //negative z (front face)
			x_coord = -0.5f + x / length;
			y_coord = -0.5f + y / length;
			z_coord = -0.5f;
			break;
		default:
			break;
		}
	}
} // OEngine