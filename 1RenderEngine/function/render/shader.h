#pragma once

#include "../../resource/texture.h"
#include "../../resource/tgaimage.h"
#include "../../core/math/math_headers.h"
#include "../../resource/model.h"
#include ".././platform/camera.h"
#include "./light.h"
#include "../render/sampler.h"

#include <memory>

namespace OEngine
{
	static Vector3 interpolate(float alpha, float beta, float gamma, const Vector3& ver1, const Vector3& ver2, const Vector3& ver3, float weight = 1.f)
	{
		return Vector3((alpha * ver1 + beta * ver2, gamma * ver3) / weight);
	}

	static Vector2 interpolate(float alpha, float beta, float gamma, const Vector2& ver1, const Vector2& ver2, const Vector2& ver3, float weight = 1.f)
	{
		auto u = (alpha * ver1.x + beta * ver2.x + gamma * ver3.x) / weight;
		auto v = (alpha * ver1.y + beta * ver2.y + gamma * ver3.y) / weight;

		return Vector2(u, v);
	}

	typedef enum
	{
		W_PLANE,
		X_RIGHT,
		X_LEFT,
		Y_TOP,
		Y_BOTTOM,
		Z_NEAR,
		Z_FAR
	} clip_plane;

	static int is_inside_plane(clip_plane c_plane, Vector4 vertex)
	{
		switch (c_plane)
		{
		case W_PLANE:
			return vertex.w <= -Float_EPSILON;
		case X_RIGHT:
			return vertex.x >= vertex.w;
		case X_LEFT:
			return vertex.x<= -vertex.w;
		case Y_TOP:
			return vertex.y >= vertex.w;
		case Y_BOTTOM:
			return vertex.y <= -vertex.w;
		case Z_NEAR:
			return vertex.z >= vertex.w;
		case Z_FAR:
			return vertex.z <= -vertex.w;
		default:
			return 0;
		}
	}

	static float get_intersect_ratio(Vector4 prev, Vector4 curv, clip_plane c_plane)
	{
		switch (c_plane)
		{
		case W_PLANE:
			return (prev.w + Float_EPSILON) / (prev.w - curv.w);
		case X_RIGHT:
			return (prev.w - prev.x) / ((prev.w - prev.x) - (curv.w - curv.x));
		case X_LEFT:
			return (prev.w + prev.x) / ((prev.w + prev.x) - (curv.w + curv.x));
		case Y_TOP:
			return (prev.w - prev.y) / ((prev.w - prev.y) - (curv.w - curv.y));
		case Y_BOTTOM:
			return (prev.w + prev.y) / ((prev.w + prev.y) - (curv.w + curv.y));
		case Z_NEAR:
			return (prev.w - prev.z) / ((prev.w - prev.z) - (curv.w - curv.z));
		case Z_FAR:
			return (prev.w + prev.z) / ((prev.w + prev.z) - (curv.w + curv.z));
		default:
			return 0;
		}
	}

	struct payload
	{
		Vector4 in_clipPos[3];
		Vector3 in_worldPos[3];
		Vector3 in_normal[3];
		Vector2 in_texCoords[3];

		Model::Ptr model;
		Camera::Ptr camera;

		Vector4 out_clipPos[3];
		Vector3 out_worldPos[3];
		Vector3 out_normal[3];
		Vector2 out_texCoords[3];

		// vertex attribute
		Vector4 clipCoord_attri[3];
		Vector3 worldCoord_attri[3];
		Vector3 normal_attri[3];
		Vector2 uv_attri[3];
	};

	static void transform_attri(payload& pl, int ind0, int ind1, int ind2)
	{
		pl.clipCoord_attri[0]	= pl.out_clipPos[ind0];
		pl.clipCoord_attri[1]	= pl.out_clipPos[ind1];
		pl.clipCoord_attri[2]	= pl.out_clipPos[ind2];
		pl.worldCoord_attri[0]	= pl.out_worldPos[ind0];
		pl.worldCoord_attri[1]	= pl.out_worldPos[ind1];
		pl.worldCoord_attri[2]	= pl.out_worldPos[ind2];
		pl.normal_attri[0]		= pl.out_normal[ind0];
		pl.normal_attri[1]		= pl.out_normal[ind1];
		pl.normal_attri[2]		= pl.out_normal[ind2];
		pl.uv_attri[0]			= pl.out_texCoords[ind0];
		pl.uv_attri[1]			= pl.out_texCoords[ind1];
		pl.uv_attri[2]			= pl.out_texCoords[ind2];
	}

	// homo clip
	static int clipWithPlane(clip_plane c_plane, int num_vert, payload& pl)
	{
		int out_vert_num = 0;
		int preInd, curInd;
		int is_odd = (c_plane + 1) % 2;

		Vector4* in_clipcoord	= is_odd ? pl.in_clipPos : pl.out_clipPos;
		Vector3* in_worldcoord	= is_odd ? pl.in_worldPos : pl.out_worldPos;
		Vector3* in_normal		= is_odd ? pl.in_normal : pl.out_normal;
		Vector2* in_uv			= is_odd ? pl.in_texCoords : pl.out_texCoords;
		Vector4* out_clipcoord	= is_odd ? pl.out_clipPos : pl.in_clipPos;
		Vector3* out_worldcoord	= is_odd ? pl.out_worldPos : pl.in_worldPos;
		Vector3* out_normal		= is_odd ? pl.out_normal : pl.in_normal;
		Vector2* out_uv			= is_odd ? pl.out_texCoords : pl.in_texCoords;


		for (int i = 0; i < num_vert; i++)
		{
			int curInd = i;
			int preInd = (i - 1 + num_vert) % num_vert;

			Vector4 cur_vertex = in_clipcoord[curInd];
			Vector4 pre_vertex = in_clipcoord[preInd];

			int is_cur_inside = is_inside_plane(c_plane, cur_vertex);
			int is_pre_inside = is_inside_plane(c_plane, pre_vertex);

			if (is_cur_inside != is_pre_inside)
			{
				float ratio = get_intersect_ratio(pre_vertex, cur_vertex, c_plane);

				out_clipcoord[out_vert_num]		= Vector4::lerp(pre_vertex, cur_vertex, ratio);
				out_worldcoord[out_vert_num]	= Vector3::lerp(in_worldcoord[preInd], in_worldcoord[curInd], ratio);
				out_normal[out_vert_num]		= Vector3::lerp(in_normal[preInd], in_normal[curInd], ratio);
				out_uv[out_vert_num]			= Vector2::lerp(in_uv[preInd], in_uv[curInd], ratio);
				
				out_vert_num++;
			}

			if (is_cur_inside)
			{
				out_clipcoord[out_vert_num]		= cur_vertex;
				out_worldcoord[out_vert_num]	= in_worldcoord[curInd];
				out_normal[out_vert_num]		= in_normal[curInd];
				out_uv[out_vert_num]			= in_uv[curInd];
				
				out_vert_num++;
			}
		}

		return out_vert_num;
	}

	static int homoClipping(payload& pl)
	{
		int num_vertex = 3;
		num_vertex = clipWithPlane(W_PLANE, num_vertex, pl);
		num_vertex = clipWithPlane(X_RIGHT, num_vertex, pl);
		num_vertex = clipWithPlane(X_LEFT, num_vertex, pl);
		num_vertex = clipWithPlane(Y_TOP, num_vertex, pl);
		num_vertex = clipWithPlane(Y_BOTTOM, num_vertex, pl);
		num_vertex = clipWithPlane(Z_NEAR, num_vertex, pl);
		num_vertex = clipWithPlane(Z_FAR, num_vertex, pl);
		return num_vertex;
	}

	struct fragment_shader_payload
	{
		fragment_shader_payload() {}

		fragment_shader_payload(const Vector3& col, const Vector3& nor, const Vector2& uv)
			: color(col), normal(nor), tex_coords(uv) {}

		Vector3 view_pos;
		Vector3 color;
		Vector3 normal;
		Vector2 tex_coords;
	};

	struct vertex_shader_payload
	{
		Vector3 position;
	};

	class ShaderProgram
	{
	public:
		typedef std::shared_ptr<ShaderProgram> Ptr;

		payload m_payload;

		Light m_light;

		Matrix4x4 m_model			= Matrix4x4::IDENTITY;
		Matrix4x4 m_view			= Matrix4x4::IDENTITY;
		Matrix4x4 m_projection		= Matrix4x4::IDENTITY;
		Matrix4x4 m_mvp				= Matrix4x4::IDENTITY;

	public:
		virtual void vertex_shader(int nfaces, int nvertex) {}
		virtual Vector3 fragment_shader(float alpha, float gamma, float beta) { return Vector3(255, 255, 255); }

		inline void set_model(const Matrix4x4& model) { m_model = model; }
		inline void set_view(const Matrix4x4& view) { m_view = view; }
		inline void set_projection(const Matrix4x4& projection) { m_projection = projection; }
	};

	class PhongShader : public ShaderProgram
	{
	public:
		typedef std::shared_ptr<PhongShader> Ptr;

		void vertex_shader(int nfaces, int nvertex);
		Vector3 fragment_shader(float alpha, float gamma, float beta);
	};

	class SkyBoxShader : public ShaderProgram
	{
	public:
		typedef std::shared_ptr<SkyBoxShader> Ptr;

		void vertex_shader(int nfaces, int nvertex);
		Vector3 fragment_shader(float alpha, float gamma, float beta);
	};

	class PBRShader : public ShaderProgram
	{
	public:
		typedef std::shared_ptr<PBRShader> Ptr;

		void vertex_shader(int nfaces, int nvertex);
		Vector3 fragment_shader(float alpha, float gamma, float beta);
	};
} // OEngine
