#include "./rasterizer.h"
#include "../../core/math/math_headers.h"

#include <opencv2/opencv.hpp>
#include <math.h>
#include <algorithm>
#include <tuple>

namespace OEngine
{
	void Rasterizer::draw_line(Vector3 begin, Vector3 end)
	{
		auto x1 = begin.x;
		auto y1 = begin.y;
		auto x2 = end.x;
		auto y2 = end.y;

		Vector3 line_color = { 255, 255, 255 };

		int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;

		dx = x2 - x1;
		dy = y2 - y1;
		dx1 = fabs(dx);
		dy1 = fabs(dy);
		px = 2 * dy1 - dx1;
		py = 2 * dx1 - dy1;

		if (dy1 <= dx1)
		{
			if (dx >= 0)
			{
				x = x1;
				y = y1;
				xe = x2;
			}
			else
			{
				x = x2;
				y = y2;
				xe = x1;
			}
			Vector2 point = Vector2((int)x, (int)y);
			set_pixel(point, line_color);
			for (i = 0; x < xe; i++)
			{
				x = x + 1;
				if (px < 0)
				{
					px = px + 2 * dy1;
				}
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
					{
						y = y + 1;
					}
					else
					{
						y = y - 1;
					}
					px = px + 2 * (dy1 - dx1);
				}
				Vector2 point = Vector2((int)x, (int)y);
				set_pixel(point, line_color);
			}
		}
		else
		{
			if (dy >= 0)
			{
				x = x1;
				y = y1;
				ye = y2;
			}
			else
			{
				x = x2;
				y = y2;
				ye = y1;
			}
			Vector2 point = Vector2((int)x, (int)y);
			set_pixel(point, line_color);
			for (i = 0; y < ye; i++)
			{
				y = y + 1;
				if (py <= 0)
				{
					py = py + 2 * dx1;
				}
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
					{
						x = x + 1;
					}
					else
					{
						x = x - 1;
					}
					py = py + 2 * (dx1 - dy1);
				}
				Vector2 point = Vector2((int)x, (int)y);
				set_pixel(point, line_color);
			}
		}
	}

	static bool isBackFacing(const std::vector<Vector3>& ndcPos)
	{
		Vector3 a = ndcPos[0];
		Vector3 b = ndcPos[1];
		Vector3 c = ndcPos[2];
		float flag = a.x * b.y - a.y * b.x +
			b.x * c.y - b.y * c.x +
			c.x * a.y - c.y * a.x;
		return flag <= 0.f;
	}

	static bool insideTriangle(int x, int y, const Vector4* _v)
	{
		Vector2 PA = Vector2{ x - _v[0].x, y - _v[0].y };
		Vector2 PB = Vector2{ x - _v[1].x, y - _v[1].y };
		Vector2 PC = Vector2{ x - _v[2].x, y - _v[2].y };
		
		float a = PA.crossProduct(PB);
		float b = PB.crossProduct(PC);
		float c = PC.crossProduct(PA);

		return (a * b >= 0) && (a * c >= 0) && (b * c >= 0);
	}

	static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector4* v)
	{
		float c1 = (x * (v[1].y - v[2].y) + (v[2].x - v[1].x) * y + v[1].x * v[2].y - v[2].x * v[1].y) / (v[0].x * (v[1].y - v[2].y) + (v[2].x - v[1].x) * v[0].y + v[1].x * v[2].y - v[2].x * v[1].y);
		float c2 = (x * (v[2].y - v[0].y) + (v[0].x - v[2].x) * y + v[2].x * v[0].y - v[0].x * v[2].y) / (v[1].x * (v[2].y - v[0].y) + (v[0].x - v[2].x) * v[1].y + v[2].x * v[0].y - v[0].x * v[2].y);
		float c3 = (x * (v[0].y - v[1].y) + (v[1].x - v[0].x) * y + v[0].x * v[1].y - v[1].x * v[0].y) / (v[2].x * (v[0].y - v[1].y) + (v[1].x - v[0].x) * v[2].y + v[0].x * v[1].y - v[1].x * v[0].y);
		return { c1, c2, c3 };
	}

	void Rasterizer::draw(std::vector<Triangle*>& TriangleList)
	{
		float f1 = (50 - 0.1) / 2.0;
		float f2 = (50 + 0.1) / 2.0;

		Matrix4x4 mvp = m_projection * m_view * m_model;
		for (const auto& t : TriangleList)
		{
			Triangle tri = *t;

			auto tp1 = m_view * m_model * t->m_vertices[0];
			auto tp2 = m_view * m_model * t->m_vertices[1];
			auto tp3 = m_view * m_model * t->m_vertices[2];
			
			std::vector<Vector3> viewPos{
				Vector3(tp1.x, tp1.y, tp1.z),
				Vector3(tp1.x, tp1.y, tp1.z),
				Vector3(tp1.x, tp1.y, tp1.z)
			};
			
			if (isBackFacing(viewPos)) continue;

			Vector4 v[] = {
				mvp * t->m_vertices[0],
				mvp * t->m_vertices[1],
				mvp * t->m_vertices[2]
			};

			// 处理齐次项
			for (auto& vec : v)
			{
				vec.x /= vec.w;
				vec.y /= vec.w;
				vec.z /= vec.w;
			}

			Matrix4x4 inv_trans = (m_view * m_model).inverse().tranpose();

			Vector4 n[] = {
				inv_trans * Vector4(t->m_normals[0], 0),
				inv_trans * Vector4(t->m_normals[1], 0),
				inv_trans * Vector4(t->m_normals[2], 0)
			};

			for (auto& vert : v)
			{
				vert.x = 0.5 * m_width * (vert.x + 1.0);
				vert.y = 0.5 * m_height * (vert.y + 1.0);
				vert.z = vert.z * f1 + f2;
			}

			for (int i = 0; i < 3; i++)
			{
				tri.setVertex(i, v[i]);
			}

			for (int i = 0; i < 3; i++)
			{
				tri.setNormal(i, Vector3(n[i].x, n[i].y, n[i].z));
			}

			tri.setColor(0, 148.f, 121.f, 92.f);
			tri.setColor(1, 148.f, 121.f, 92.f);
			tri.setColor(2, 148.f, 121.f, 92.f);

			rasterize_triangle(tri, viewPos);
		}
		
	}

	static Vector3 interpolate(float alpha, float beta, float gamma, const Vector3& ver1, const Vector3& ver2, const Vector3& ver3, float weight)
	{
		return Vector3((alpha * ver1 + beta * ver2, gamma * ver3) / weight);
	}

	static Vector2 interpolate(float alpha, float beta, float gamma, const Vector2& ver1, const Vector2& ver2, const Vector2& ver3, float weight)
	{
		auto u = (alpha * ver1.x + beta * ver2.x + gamma * ver3.x) / weight;
		auto v = (alpha * ver1.y + beta * ver2.y + gamma * ver3.y) / weight;

		return Vector2(u, v);
	}

	void Rasterizer::draw(const Model& model)
	{
		float f1 = (50 - 0.1) / 2.0;
		float f2 = (50 + 0.1) / 2.0;

		Matrix4x4 mvp = m_projection * m_view * m_model;
		for (int i = 0; i < model.nfaces(); i++)
		{
			Triangle t;
			for (int j = 0; j < 3; j++)
			{
				t.m_vertices[j] = Vector4(model.m_verts[model.m_faces[i][j * 3]]);
				t.m_normals[j] = model.m_norms[model.m_faces[i][j * 3 + 1]];
				t.m_tex_coords[j] = model.m_uvs[model.m_faces[i][j * 3 + 2]];
			}
			
			auto tp1 = m_view * m_model * t.m_vertices[0];
			auto tp2 = m_view * m_model * t.m_vertices[1];
			auto tp3 = m_view * m_model * t.m_vertices[2];

			std::vector<Vector3> viewPos{
				Vector3(tp1.x, tp1.y, tp1.z),
				Vector3(tp1.x, tp1.y, tp1.z),
				Vector3(tp1.x, tp1.y, tp1.z)
			};

			Vector4 v[] = {
				mvp * t.m_vertices[0],
				mvp * t.m_vertices[1],
				mvp * t.m_vertices[2]
			};

			// 处理齐次项
			for (auto& vec : v)
			{
				vec.x /= vec.w;
				vec.y /= vec.w;
				vec.z /= vec.w;
			}

			Matrix4x4 inv_trans = (m_view * m_model).inverse().tranpose();

			Vector4 n[] = {
				inv_trans * Vector4(t.m_normals[0], 0),
				inv_trans * Vector4(t.m_normals[1], 0),
				inv_trans * Vector4(t.m_normals[2], 0)
			};

			for (auto& vert : v)
			{
				vert.x = 0.5 * m_width * (vert.x + 1.0);
				vert.y = 0.5 * m_height * (vert.y + 1.0);
				vert.z = vert.z * f1 + f2;
			}

			for (int i = 0; i < 3; i++)
			{
				t.setVertex(i, v[i]);
			}

			for (int i = 0; i < 3; i++)
			{
				t.setNormal(i, Vector3(n[i].x, n[i].y, n[i].z));
			}

			t.setColor(0, 148.f, 121.f, 92.f);
			t.setColor(1, 148.f, 121.f, 92.f);
			t.setColor(2, 148.f, 121.f, 92.f);

			rasterize_triangle(t, viewPos);
		}
	}

	void Rasterizer::rasterize_triangle(const Triangle& t, const std::vector<Vector3>& worldPos)
	{
		auto v = t.toVector4();

		unsigned char color[3];

		// 光栅化过程
		float xlhs = Math::min3(t.m_vertices[0].x, t.m_vertices[1].x, t.m_vertices[2].x);
		float xrhs = Math::max3(t.m_vertices[0].x, t.m_vertices[1].x, t.m_vertices[2].x);
		float ybot = Math::min3(t.m_vertices[0].y, t.m_vertices[1].y, t.m_vertices[2].y);
		float ycel = Math::max3(t.m_vertices[0].y, t.m_vertices[1].y, t.m_vertices[2].y);

		// std::cout << xlhs << " -> " << xrhs << "----" << ybot << " -> " << ycel << '\n';

		for (int x = xlhs; x <= xrhs; x++)
		{
			for (int y = ybot; y <= ycel; y++)
			{
				if (insideTriangle(x, y, t.m_vertices))
				{
					// std::cout << "inside compute..." << '\n';

					auto [alpha, gamma, beta] = computeBarycentric2D(x, y, t.m_vertices);
					float Z = 1.f / (alpha / t.m_vertices[0].w + beta / t.m_vertices[1].w + gamma / t.m_vertices[2].w);
					float zp = alpha * t.m_vertices[0].z / t.m_vertices[0].w
							+ beta * t.m_vertices[1].z / t.m_vertices[1].w
							+ gamma * t.m_vertices[2].z / t.m_vertices[2].w;
					zp *= Z;

					// depth test
					/*
					*  interpolate_color			: 颜色插值
					*  interpolate_normal			：法向量插值
					*  interpolate_texcoords		: 贴图采样坐标插值
					*  interpolate_shadingcoords	: 着色坐标插值
					*/
					if (zp < m_depth_buf[get_index(x, y)])
					{
						auto interpolate_color = interpolate(alpha, beta, gamma, t.m_colors[0], t.m_colors[1], t.m_colors[2], 1);
						auto interpolate_normal = interpolate(alpha, beta, gamma, t.m_normals[0], t.m_normals[1], t.m_normals[2], 1);
						auto interpolate_texcoords = interpolate(alpha, beta, gamma, t.m_tex_coords[0], t.m_tex_coords[1], t.m_tex_coords[2], 1);
						auto interpolate_shadingcoords = interpolate(alpha, beta, gamma, worldPos[0], worldPos[1], worldPos[2], 1);

						fragment_shader_payload payload(interpolate_color, interpolate_normal, interpolate_texcoords, m_texture ? &*m_texture : nullptr);
						payload.view_pos = interpolate_shadingcoords;
						auto pixel_color = fragment_shader(payload);
						set_pixel(Vector2((int)x, (int)y), pixel_color);
						m_depth_buf[get_index(x, y)] = zp;
					}
				}
			}
		}
	}

	void Rasterizer::set_model(const Matrix4x4& m)
	{
		m_model = m;
	}

	void Rasterizer::set_view(const Matrix4x4& v)
	{
		m_view = v;
	}

	void Rasterizer::set_projection(const Matrix4x4& p)
	{
		m_projection = p;
	}

	void Rasterizer::clear(Buffers buff)
	{
		if ((buff & Buffers::Color) == Buffers::Color)
		{
			std::fill(m_frame_buf.begin(), m_frame_buf.end(), Vector3{ 0, 0, 0 });
		}
		if ((buff & Buffers::Depth) == Buffers::Depth)
		{
			std::fill(m_depth_buf.begin(), m_depth_buf.end(), std::numeric_limits<float>::infinity());
		}
	}

	Rasterizer::Rasterizer(int w, int h) : m_width(w), m_height(h)
	{
		m_frame_buf.resize(w * h);
		m_depth_buf.resize(w * h);
		m_texture = std::nullopt;
	}

	void Rasterizer::set_vertex_shader(std::function<Vector3(vertex_shader_payload)> vs)
	{
		vertex_shader = vs;
	}

	void Rasterizer::set_fragment_shader(std::function<Vector3(fragment_shader_payload)> fs)
	{
		fragment_shader = fs;
	}

	int Rasterizer::get_index(int x, int y)
	{
		return (m_height - y) * m_width + x;
	}

	void Rasterizer::set_pixel(const Vector2& point, const Vector3& color)
	{
		int ind = (m_height - point.y) * m_width + point.x;
		// std::cout << "pixel color in [" << ind << ']' << "RGB: " << color.x << color.y << color.z;
		m_frame_buf[ind] = color;
	}

} // OEngine