#include "triangle.h"
#include <algorithm>
#include <iostream>
#include <array>

namespace OEngine
{
	/*
	*  为什么不初始化法线序列
	*/
	Triangle::Triangle()
	{
		m_vertices[0] = Vector4{ 0, 0, 0, 1 };
		m_vertices[0] = Vector4{ 0, 0, 0, 1 };
		m_vertices[0] = Vector4{ 0, 0, 0, 1 };

		m_colors[0] = Vector3::UNIT_SCALE;
		m_colors[1] = Vector3::UNIT_SCALE;
		m_colors[2] = Vector3::UNIT_SCALE;

		m_tex_coords[0] = Vector2{ 0, 0 };
		m_tex_coords[1] = Vector2{ 0, 0 };
		m_tex_coords[2] = Vector2{ 0, 0 };
	}

	void Triangle::setVertex(int ind, Vector4 v) { m_vertices[ind] = v; }

	void Triangle::setNormal(int ind, Vector3 n) { m_normals[ind] = n; }

	void Triangle::setColor(int ind, float r, float g, float b)
	{
		if (((r < 0.0) || (r > 255.0)) ||
			((g < 0.0) || (g > 255.0)) ||
			((b < 0.0) || (b > 255.0)))
		{
			std::cerr << "ERROR::COLOR_FORMAT" << std::endl;
			fflush(stderr);
			exit(-1);
		}
		m_colors[ind] = Vector3((float)r / 255., (float)g / 255., (float)b / 255.);
		return;
	}

	void Triangle::setTexCoord(int ind, Vector2 uv)
	{
		m_tex_coords[ind] = uv;
	}

	std::vector<Vector4> Triangle::toVector4() const
	{
		std::vector<Vector4> res(3, Vector4::ZERO);
		std::transform(std::begin(m_vertices), std::end(m_vertices), res.begin(),
			[](auto& v) { return Vector4(v.x, v.y, v.z, 1.); });
		return res;
	}

	void Triangle::setNormals(const std::vector<Vector3>& normals)
	{
		m_normals[0] = normals[0];
		m_normals[1] = normals[1];
		m_normals[2] = normals[2];
	}

	// TODO
	void Triangle::setColors(const std::vector<Vector3>& colors)
	{
		m_colors[0] = colors[0];
		m_colors[1] = colors[1];
		m_colors[2] = colors[2];
	}

} // OEngine