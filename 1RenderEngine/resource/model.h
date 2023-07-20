#pragma once

#include <string>
#include <vector>

#include "../core/math/math_headers.h"
#include "../function/render/shader.h"
#include "./tgaimage.h"

namespace OEngine
{
	typedef struct cubemap cubemap_t;

	class Model
	{
		friend class Rasterizer;
	private:
		std::vector<Vector3> m_verts;
		std::vector<std::vector<int> > m_faces; // vertex/uv/normal
		std::vector<Vector3> m_norms;
		std::vector<Vector2> m_uvs;

		void load_cubemap(const char* filename);
		void create_map(const char* filename);
		void load_texture(std::string filename, const char* suffix, TGAImage* img);
		void load_texture(std::string filename, const char* suffix, TGAImage& img);

	public:
		Model(const char* filename, int is_skybox = 0);
		~Model();
		
		cubemap_t* environment_map;
		int is_skybox;

		TGAImage* diffuse_map;
		TGAImage* normal_map;
		TGAImage* specular_map;
		TGAImage* roughness_map;
		TGAImage* metalness_map;
		TGAImage* occlusion_map;
		TGAImage* emision_map;

		int nverts() const;
		int nfaces() const;
		Vector3 normal(int iface, int nthvert);
		Vector3 normal(Vector2 uv);
		Vector3 vert(int i);
		Vector3 vert(int iface, int nthvert);

		Vector2 uv(int iface, int nthvert);
		Vector3 diffuse(Vector2 uv);
		float roughness(Vector2 uv);
		float metalness(Vector2 uv);
		float specular(Vector2 uv);
		float occlusion(Vector2 uv);
		Vector3 emission(Vector2 uv);

		std::vector<int> face(int idx);
	};
} // OEngine