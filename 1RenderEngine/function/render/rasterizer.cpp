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

	static bool isBackFacing(Vector3 ndcPos[3])
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

	static bool insideTriangle(int x, int y, const Vector3* _v)
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

	static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector3* v)
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


	void Rasterizer::draw(Model::Ptr model, ShaderProgram::Ptr shader)
	{
		float f1 = (50 - 0.1) / 2.0;
		float f2 = (50 + 0.1) / 2.0;
		/*
		* TODO: 1. shader 相关参数的设置
		*		2. 以face为单位, 处理每个片元的相关数据，将数据存放在
		*				-in_clipPos		(经过mvp变换的windowPos)
		*				-in_worldPos	(经过model矩阵变换的世界坐标系下的坐标)
		*				-in_normal		(通过model的normal_map获取 或 片元中自带的)
		*				-in_uv			(片元自带数据)
		*		3. 顶点数据处理(整合至shader中的vertex_shader中)
		*		4. 齐次裁剪 放在 齐次项处理之前
		*		5. 三角光栅化   
		*/
		for (int i = 0; i < model->nfaces(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				shader->vertex_shader(i, j);
			}

			int num_vertex = 3;
			// TODO
			if (!model->is_skybox) num_vertex = homoClipping(shader->m_payload);

			for (int k = 0; k < num_vertex - 2; k++)
			{
				int ind0 = 0;
				int ind1 = k + 1;
				int ind2 = k + 2;

				transform_attri(shader->m_payload, ind0, ind1, ind2);
				rasterize_triangle(shader, model);
			}
		}
	}

	void Rasterizer::rasterize_triangle(const Triangle& t, const std::vector<Vector3>& worldPos)
	{
		auto v = t.toVector4();

		unsigned char color[3];

		// 光栅化过程
		float xlhs = std::min(t.m_vertices[0].x, std::min(t.m_vertices[1].x, t.m_vertices[2].x));
		float xrhs = std::max(t.m_vertices[0].x, std::max(t.m_vertices[1].x, t.m_vertices[2].x));
		float ybot = std::min(t.m_vertices[0].y, std::min(t.m_vertices[1].y, t.m_vertices[2].y));
		float ycel = std::max(t.m_vertices[0].y, std::max(t.m_vertices[1].y, t.m_vertices[2].y));

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

					// depth test  整合插值部分
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

						// auto pixel_color = fragment_shader(payload);
						// set_pixel(Vector2((int)x, (int)y), pixel_color);
						m_depth_buf[get_index(x, y)] = zp;
					}
				}
			}
		}
	}

	void Rasterizer::rasterize_triangle(ShaderProgram::Ptr shader, Model::Ptr model)
	{
		Vector3 ndcPos[3];
		Vector3 windowPos[3];

		// 去除齐次项
		for (int i = 0; i < 3; i++)
		{
			ndcPos[i].x = shader->m_payload.clipCoord_attri[i].x / shader->m_payload.clipCoord_attri[i].w;
			ndcPos[i].y = shader->m_payload.clipCoord_attri[i].y / shader->m_payload.clipCoord_attri[i].w;
			ndcPos[i].z = shader->m_payload.clipCoord_attri[i].z / shader->m_payload.clipCoord_attri[i].w;
		}

		// 视窗变化
		for (int i = 0; i < 3; i++)
		{
			windowPos[i].x = 0.5 * m_width * (ndcPos[i].x + 1.f);
			windowPos[i].y = 0.5 * m_height * (ndcPos[i].y + 1.f);
			windowPos[i].z = -(shader->m_payload.clipCoord_attri[i].w);
		}

		if (!model->is_skybox)
		{
			if (isBackFacing(ndcPos))
				return;
		}

		// 光栅化过程
		float xlhs = std::min(windowPos[0].x, std::min(windowPos[1].x, windowPos[2].x));
		float xrhs = std::max(windowPos[0].x, std::max(windowPos[1].x, windowPos[2].x));
		float ybot = std::min(windowPos[0].y, std::min(windowPos[1].y, windowPos[2].y));
		float ycel = std::max(windowPos[0].y, std::max(windowPos[1].y, windowPos[2].y));

		// std::cout << xlhs << " -> " << xrhs << "----" << ybot << " -> " << ycel << '\n';

		for (int x = xlhs; x <= xrhs; x++)
		{
			for (int y = ybot; y <= ycel; y++)
			{
				int ind = get_index(x, y);

				if (ind < 0 || ind >= m_depth_buf.size()) continue;

				if (insideTriangle(x, y, windowPos))
				{
					// std::cout << "inside compute..." << '\n';
					auto [alpha, gamma, beta] = computeBarycentric2D(x, y, windowPos);
					float Z = 1.f / (alpha / shader->m_payload.clipCoord_attri[0].w + beta / shader->m_payload.clipCoord_attri[1].w + gamma / shader->m_payload.clipCoord_attri[2].w);
					float zp = alpha * shader->m_payload.clipCoord_attri[0].z / shader->m_payload.clipCoord_attri[0].w
						+ beta * shader->m_payload.clipCoord_attri[1].z / shader->m_payload.clipCoord_attri[1].w
						+ gamma * shader->m_payload.clipCoord_attri[2].z / shader->m_payload.clipCoord_attri[2].w;
					zp *= Z;
					
					if (zp < m_depth_buf[ind])
					{
						m_depth_buf[ind] = zp;

						Vector3 color = shader->fragment_shader(alpha, gamma, beta);

						Vector3 pixel_color = color.clamp(color, Vector3(0, 0, 0), Vector3(255.f, 255.f, 255.f));
						set_pixel(Vector2((int)x, (int)y), pixel_color);
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
	}

	int Rasterizer::get_index(int x, int y)
	{
		return y * m_width + x;
	}

	void Rasterizer::set_pixel(const Vector2& point, const Vector3& color)
	{
		// 去除超出屏幕范围的点
		if (point.x < 0 || point.x > m_width
			|| point.y < 0 || point.y > m_height) return;

		int ind = point.y * m_width + point.x;
		// std::cout << "pixel color in [" << ind << ']' << "RGB: " << color.x << color.y << color.z;
		m_frame_buf[ind] = color;
	}

} // OEngine