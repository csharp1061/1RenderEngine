#pragma once

#include "../../core/math/math_headers.h"
#include "../../resource/texture.h"
#include "../../resource/model.h"
#include "./shader.h"

#include <optional>
#include <functional>
#include <memory>

namespace OEngine
{
	enum class Buffers
	{
		Color = 1,
		Depth = 2
	};

	inline Buffers operator|(Buffers a, Buffers b)
	{
		return Buffers((int)a | (int)b);
	}

	inline Buffers operator&(Buffers a, Buffers b)
	{
		return Buffers((int)a & (int)b);
	}

	class Rasterizer
	{
	public:
		typedef std::shared_ptr<Rasterizer> Ptr;

		int m_width, m_height;

		Rasterizer(int w, int h);

		void set_model(const Matrix4x4& m);
		void set_view(const Matrix4x4& v);
		void set_projection(const Matrix4x4& p);

		void set_pixel(const Vector2& point, const Vector3& color);

		void clear(Buffers buffer);

		void draw(std::vector<Triangle*>& TriangleList);
		void draw(Model::Ptr model, ShaderProgram::Ptr shader);

		std::vector<Vector3>& frame_buffer() { return m_frame_buf; }

	private:
		void draw_line(Vector3 begin, Vector3 end);

		void rasterize_triangle(const Triangle& t, const std::vector<Vector3>& worldPos);
		void rasterize_triangle(ShaderProgram::Ptr shader, Model::Ptr model);

	private:
		Matrix4x4 m_model;
		Matrix4x4 m_view;
		Matrix4x4 m_projection;

		std::vector<Vector3> m_frame_buf;
		std::vector<float>	 m_depth_buf;

		int get_index(int x, int y);
	};
} // OEngine

