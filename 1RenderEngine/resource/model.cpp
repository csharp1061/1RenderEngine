#include "./model.h"

#include <io.h>
#include <iostream>
#include <fstream>
#include <sstream>

namespace OEngine
{
	Model::Model(const char* filename, int is_skyb) : is_skybox(is_skyb)
	{
		std::ifstream in;
		in.open(filename, std::ifstream::in);
		if (in.fail())
		{
			std::cerr << "model load failed..." << filename << '\n';
			return;
		}

		std::string line;
		while (!in.eof())
		{
			std::getline(in, line);
			std::istringstream iss(line.c_str());
			char trash;
			if (!line.compare(0, 2, "v "))
			{
				iss >> trash;
				Vector3 v;
				for (int i = 0; i < 3; i++)
					iss >> v[i];
				m_verts.push_back(v);
			}
			else if (!line.compare(0, 3, "vn "))
			{
				iss >> trash >> trash;
				Vector3 n;
				for (int i = 0; i < 3; i++)
					iss >> n[i];
				m_norms.push_back(n);
			}
			else if (!line.compare(0, 3, "vt "))
			{
				iss >> trash >> trash;
				Vector2 uv;
				for (int i = 0; i < 2; i++)
					iss >> uv[i];
				m_uvs.push_back(uv);
			}
			else if (!line.compare(0, 2, "f "))
			{
				std::vector<int> f;
				int tmp[3];
				iss >> trash;
				while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2])
				{
					for (int i = 0; i < 3; i++)
						tmp[i]--;
					f.push_back(tmp[0]);
					f.push_back(tmp[1]);
					f.push_back(tmp[2]);
				}
				m_faces.push_back(f);
			}
		}
		std::cerr << "# v#" << m_verts.size() << " f# " << m_faces.size() << " #vt " << m_uvs.size()
			<< " vn# " << m_norms.size() << std::endl;

		create_map(filename);

		environment_map = NULL;
		if (is_skybox)
		{
			environment_map = new cubemap_t();
			load_cubemap(filename);
		}
	}

	Model::~Model()
	{
		if (diffuse_map)	delete diffuse_map;		diffuse_map = NULL;
		if (normal_map)		delete normal_map;		normal_map = NULL;
		if (specular_map)	delete specular_map;	specular_map = NULL;
		if (roughness_map)	delete roughness_map;	roughness_map = NULL;
		if (metalness_map)	delete metalness_map;	metalness_map = NULL;
		if (occlusion_map)	delete occlusion_map;	occlusion_map = NULL;
		if (emision_map)	delete emision_map;		emision_map = NULL;

		if (environment_map)
		{
			for (int i = 0; i < 6; i++)
				delete environment_map->faces[i];
			delete environment_map;
		}
	}

	void Model::create_map(const char* filename)
	{
		diffuse_map		= NULL;
		normal_map		= NULL;
		specular_map	= NULL;
		roughness_map	= NULL;
		metalness_map	= NULL;
		occlusion_map	= NULL;
		emision_map		= NULL;

		std::string texfile(filename);
		size_t dot = texfile.find_last_of(".");

		// diffuse_map
		texfile = texfile.substr(0, dot) + std::string("_diffuse.tga");
		if (_access(texfile.data(), 0) != -1)
		{
			diffuse_map = new TGAImage();
			load_texture(filename, "_diffuse.tga", diffuse_map);
		}

		// normal_map
		texfile = texfile.substr(0, dot) + std::string("_normal.tga");
		if (_access(texfile.data(), 0) != -1)
		{
			normal_map = new TGAImage();
			load_texture(filename, "_normal.tga", normal_map);
		}

		// specular_map
		texfile = texfile.substr(0, dot) + std::string("_spec.tga");
		if (_access(texfile.data(), 0) != -1)
		{
			specular_map = new TGAImage();
			load_texture(filename, "_spec.tga", specular_map);
		}

		// roughness
		texfile = texfile.substr(0, dot) + std::string("_roughness.tga");
		if (_access(texfile.data(), 0) != -1)
		{
			roughness_map = new TGAImage();
			load_texture(filename, "_roughness.tga", roughness_map);
		}

		// metalness
		texfile = texfile.substr(0, dot) + std::string("_metalness.tga");
		if (_access(texfile.data(), 0) != -1)
		{
			metalness_map = new TGAImage();
			load_texture(filename, "_metalness.tga", metalness_map);
		}

		// emission
		texfile = texfile.substr(0, dot) + std::string("_emission.tga");
		if (_access(texfile.data(), 0) != -1)
		{
			emision_map = new TGAImage();
			load_texture(filename, "_emission.tga", emision_map);
		}

		// occlusion
		texfile = texfile.substr(0, dot) + std::string("_occlusion.tga");
		if (_access(texfile.data(), 0) != -1)
		{
			occlusion_map = new TGAImage();
			load_texture(filename, "_occlusion.tga", occlusion_map);
		}
	}

	void Model::load_cubemap(const char* filename)
	{
		environment_map->faces[0] = new TGAImage();
		load_texture(filename, "_right.tga", environment_map->faces[0]);
		environment_map->faces[1] = new TGAImage();
		load_texture(filename, "_left.tga", environment_map->faces[1]);
		environment_map->faces[2] = new TGAImage();
		load_texture(filename, "_top.tga", environment_map->faces[2]);
		environment_map->faces[3] = new TGAImage();
		load_texture(filename, "_bottom.tga", environment_map->faces[3]);
		environment_map->faces[4] = new TGAImage();
		load_texture(filename, "_back.tga", environment_map->faces[4]);
		environment_map->faces[5] = new TGAImage();
		load_texture(filename, "_front.tga", environment_map->faces[5]);
	}

	int Model::nverts() const
	{
		return m_verts.size();
	}

	int Model::nfaces() const
	{
		return m_faces.size();
	}

	std::vector<int> Model::face(int idx)
	{
		std::vector<int> f;
		for (int i = 0; i < 3; i++)
			f.push_back(m_faces[idx][i * 3]);
		return f;
	}

	Vector3 Model::vert(int i)
	{
		return m_verts[i];
	}

	// face -> [vert, norm, uv, vert, norm, uv, vert, norm, uv]
	Vector3 Model::vert(int iface, int nthvert)
	{
		return m_verts[m_faces[iface][nthvert*3]];
	}

	Vector2 Model::uv(int iface, int nthvert)
	{
		return m_uvs[m_faces[iface][nthvert * 3 + 1]];
	}

	Vector3 Model::normal(int iface, int nthvert)
	{
		return m_norms[m_faces[iface][nthvert * 3 + 2]];
	}

	void Model::load_texture(std::string filename, const char* suffix, TGAImage* img)
	{
		std::string texfile(filename);
		size_t dot = texfile.find_last_of(".");
		if (dot != std::string::npos)
		{
			texfile = texfile.substr(0, dot) + std::string(suffix);
			img->read_tga_file(texfile.c_str());
			img->flip_vertically();
		}
	}

	void Model::load_texture(std::string filename, const char* suffix, TGAImage& img)
	{
		std::string texfile(filename);
		size_t dot = texfile.find_last_of(".");
		if (dot != std::string::npos)
		{
			texfile = texfile.substr(0, dot) + std::string(suffix);
			img.read_tga_file(texfile.c_str());
			img.flip_vertically();
		}
	}

	Vector3 Model::diffuse(Vector2 uv)
	{
		uv[0] = fmod(uv[0], 1);
		uv[1] = fmod(uv[1], 1);
		int uv0 = uv[0] * diffuse_map->get_width();
		int uv1 = uv[1] * diffuse_map->get_height();
		TGAColor c = diffuse_map->get(uv0, uv1);
		Vector3 res;
		for (int i = 0; i < 3; i++)
			res[2 - i] = (float)c[i] / 255.f;
		return res;
	}

	Vector3 Model::normal(Vector2 uv)
	{
		uv[0] = fmod(uv[0], 1);
		uv[1] = fmod(uv[1], 1);
		int uv0 = uv[0] * normal_map->get_width();
		int uv1 = uv[1] * normal_map->get_height();
		TGAColor c = normal_map->get(uv0, uv1);
		Vector3 res;
		for (int i = 0; i < 3; i++)
			res[2 - i] = (float)c[i] / 255.f * 2.f - 1.f;
		return res;
	}

	float Model::roughness(Vector2 uv)
	{
		uv[0] = fmod(uv[0], 1);
		uv[1] = fmod(uv[1], 1);
		int uv0 = uv[0] * roughness_map->get_width();
		int uv1 = uv[1] * roughness_map->get_height();
		return roughness_map->get(uv0, uv1)[0] / 255.f;
	}

	float Model::metalness(Vector2 uv)
	{
		uv[0] = fmod(uv[0], 1);
		uv[1] = fmod(uv[1], 1);
		int uv0 = uv[0] * metalness_map->get_width();
		int uv1 = uv[1] * metalness_map->get_height();
		return metalness_map->get(uv0, uv1)[0] / 255.f;
	}

	float Model::specular(Vector2 uv)
	{
		uv[0] = fmod(uv[0], 1);
		uv[1] = fmod(uv[1], 1);
		int uv0 = uv[0] * specular_map->get_width();
		int uv1 = uv[1] * specular_map->get_height();
		return specular_map->get(uv0, uv1)[0] / 1.f;
	}

	float Model::occlusion(Vector2 uv)
	{
		if (!occlusion_map)
			return 1;
		uv[0] = fmod(uv[0], 1);
		uv[1] = fmod(uv[1], 1);
		int uv0 = uv[0] * occlusion_map->get_width();
		int uv1 = uv[1] * occlusion_map->get_height();
		return occlusion_map->get(uv0, uv1)[0] / 255.f;
	}

	Vector3 Model::emission(Vector2 uv)
	{
		if (!emision_map)
			return Vector3(0.f, 0.f, 0.f);
		uv[0] = fmod(uv[0], 1);
		uv[1] = fmod(uv[1], 1);
		int uv0 = uv[0] * emision_map->get_width();
		int uv1 = uv[1] * emision_map->get_height();
		TGAColor c = emision_map->get(uv0, uv1);
		Vector3 res;
		for (int i = 0; i < 3; i++)
			res[2 - i] = (float)c[i] / 255.f;
		return res;
	}
} // OEngine