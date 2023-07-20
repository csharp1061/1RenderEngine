#pragma once

#include "vector4.h"
#include "vector3.h"
#include "vector2.h"
#include <vector>



namespace OEngine
{
	/*
	* 三角形 片元 类
	*/
	class Triangle
	{
	public:
		/*
		*  vertices[ind]  :  三角形的三个顶点
		*  colors[ind]	  :  第 ind 个点的颜色值
		*  normals[ind]	  :  第 ind 个点的法线值
		*  tex_coords[ind]:  第 ind 个点的材质贴图采样坐标
		*/
		Vector4 m_vertices[3];
		Vector3 m_colors[3];
		Vector3 m_normals[3];
		Vector2 m_tex_coords[3];

		// 材质指针
		Triangle();

		Vector4 a() const { return m_vertices[0]; }
		Vector4 b() const { return m_vertices[1]; }
		Vector4 c() const { return m_vertices[2]; }

		void setVertex(int ind, Vector4 v);
		void setNormal(int ind, Vector3 n);
		void setColor(int ind, float r, float g, float b);

		void setNormals(const std::vector<Vector3>& normals);
		void setColors(const std::vector<Vector3>& colors);
		void setTexCoord(int ind, Vector2 uv);

		std::vector<Vector4> toVector4() const;
	};
} // OEngine