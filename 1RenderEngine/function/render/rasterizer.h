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

	enum class Primitive
	{
		Line,
		Triangle
	};

	struct pos_buf_id
	{
		int pos_id = 0;
	};

	struct ind_buf_id
	{
		int ind_id = 0;
	};

	struct col_buf_id
	{
		int col_id;
	};

	class Rasterizer
	{
	public:
		typedef std::shared_ptr<Rasterizer> Ptr;

		int m_width, m_height;

		Rasterizer(int w, int h);

		void set_model(const Matrix4x4& m);
		void set_view(const Matrix4x4& v);
		void set_projection(const Matrix4x4& p);

		void set_texture(Texture tex) { m_texture = tex; }

		void set_vertex_shader(std::function<Vector3(vertex_shader_payload)> vertex_shader);
		void set_fragment_shader(std::function<Vector3(fragment_shader_payload)> fragment_shader);

		void set_pixel(const Vector2& point, const Vector3& color);

		void clear(Buffers buffer);

		void draw(std::vector<Triangle*>& TriangleList);
		void draw(const Model& model);

		std::vector<Vector3>& frame_buffer() { return m_frame_buf; }

	private:
		void draw_line(Vector3 begin, Vector3 end);

		void rasterize_triangle(const Triangle& t, const std::vector<Vector3>& worldPos);

	private:
		Matrix4x4 m_model;
		Matrix4x4 m_view;
		Matrix4x4 m_projection;

		std::optional<Texture> m_texture;

		std::function<Vector3(fragment_shader_payload)> fragment_shader;
		std::function<Vector3(vertex_shader_payload)> vertex_shader;

		std::vector<Vector3> m_frame_buf;
		std::vector<float>	 m_depth_buf;

		int get_index(int x, int y);
	};
} // OEngine

