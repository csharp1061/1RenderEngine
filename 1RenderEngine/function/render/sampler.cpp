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

	void generate_prefilter_map(int thread_id, int face_id, int mip_level, Model::Ptr model, TGAImage& image)
	{
		int factor = 1;
		for (int temp = 0; temp < mip_level; temp++)
			factor *= 2;
		int width = 512 / factor;
		int height = 512 / factor;


		if (width < 64)
			width = 64;

		int x, y;

		float roughness[10];
		for (int i = 0; i < 10; i++)
			roughness[i] = i * (1.0 / 9.0);
		roughness[0] = 0; roughness[9] = 1;

		/* for multi-thread */
		//int interval = width / thread_num;
		//int start = thread_id * interval;
		//int end = (thread_id + 1) * interval;
		//if (thread_id == thread_num - 1)
		//	end = width;

		Vector3 prefilter_color(0, 0, 0);
		for (x = 0; x < height; x++)
		{
			for (y = 0; y < width; y++)
			{
				float x_coord, y_coord, z_coord;
				set_normal_coord(face_id, x, y, x_coord, y_coord, z_coord, float(width - 1));

				Vector3 normal = Vector3(x_coord, y_coord, z_coord);
				normal.normalise();					//z-axis
				Vector3 up = fabs(normal[1]) < 0.999f ? Vector3(0.0f, 1.0f, 0.0f) : Vector3(0.0f, 0.0f, 1.0f);
				Vector3 right = up.crossProduct(normal).normalizedCopy();	//x-axis
				up = normal.crossProduct(right);						//y-axis

				Vector3 r = normal;
				Vector3 v = r;

				prefilter_color = Vector3(0, 0, 0);
				float total_weight = 0.0f;
				int numSamples = 1024;
				for (int i = 0; i < numSamples; i++)
				{
					Vector2 Xi = hammersley2d(i, numSamples);
					Vector3 h = ImportanceSampleGGX(Xi, normal, roughness[mip_level]);
					Vector3 l = (2.0 * v.dotProduct(h) * h - v).normalizedCopy();

					Vector3 radiance = cubemap_sample(l, model->environment_map);
					float n_dot_l = std::max(normal.dotProduct(l), 0.f);

					if (n_dot_l > 0)
					{
						prefilter_color += radiance * n_dot_l;
						total_weight += n_dot_l;
					}
				}

				prefilter_color = prefilter_color / total_weight;
				//cout << irradiance << endl;
				int red = std::min(prefilter_color.x * 255.0f, 255.f);
				int green = std::min(prefilter_color.y * 255.0f, 255.f);
				int blue = std::min(prefilter_color.z * 255.0f, 255.f);
				//cout << irradiance << endl;
				TGAColor temp((unsigned char)red, (unsigned char)green, (unsigned char)blue);
				image.set(x, y, temp);
			}
			printf("%f% \n", x / 512.0f);
		}
	}

	void generate_irradiance_map(int thread_id, int face_id, Model::Ptr model, TGAImage& image)
	{
		int x, y;
		/*Model* model[1];
		model[0] = new Model(modelname5[1], 1);

		payload_t p;
		p.model = model[0];*/

		Vector3 irradiance(0, 0, 0);
		for (x = 0; x < 256; x++)
		{
			for (y = 0; y < 256; y++)
			{
				float x_coord, y_coord, z_coord;
				set_normal_coord(face_id, x, y, x_coord, y_coord, z_coord);
				Vector3 normal = Vector3(x_coord, y_coord, z_coord).normalizedCopy();					 //z-axis
				Vector3 up = fabs(normal[1]) < 0.999f ? Vector3(0.0f, 1.0f, 0.0f) : Vector3(0.0f, 0.0f, 1.0f);
				Vector3 right = up.crossProduct(normal).normalizedCopy();								 //tagent x-axis
				up = normal.crossProduct(right);					                                 //tagent y-axis

				irradiance = Vector3(0, 0, 0);
				float sampleDelta = 0.025f;
				int numSamples = 0;
				for (float phi = 0.0f; phi < 2.0 * Math_PI; phi += sampleDelta)
				{
					for (float theta = 0.0f; theta < 0.5 * Math_PI; theta += sampleDelta)
					{
						// spherical to cartesian (in tangent space)
						Vector3 tangentSample = Vector3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
						// tangent space to world
						Vector3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;
						sampleVec.normalise();
						Vector3 color = cubemap_sample(sampleVec, model->environment_map);

						irradiance += color * sin(theta) * cos(theta);
						numSamples++;
					}
				}

				irradiance = Math_PI * irradiance * (1.0f / numSamples);
				int red = std::min(irradiance.x * 255.0f, 255.f);
				int green = std::min(irradiance.y * 255.0f, 255.f);
				int blue = std::min(irradiance.z * 255.0f, 255.f);

				TGAColor temp(red, green, blue);
				image.set(x, y, temp);
			}
			printf("%f% \n", x / 256.0f);
		}
	}

	Vector3 IntegrateBRDF(float NdotV, float roughness)
	{
		// 由于各向同性，随意取一个 V 即可
		Vector3 V;
		V[0] = 0;
		V[1] = sqrt(1.0 - NdotV * NdotV);
		V[2] = NdotV;

		float A = 0.0;
		float B = 0.0;
		float C = 0.0;

		Vector3 N = Vector3(0.0, 0.0, 1.0);

		const int SAMPLE_COUNT = 1024;
		for (int i = 0; i < SAMPLE_COUNT; ++i)
		{
			// generates a sample vector that's biased towards the
			// preferred alignment direction (importance sampling).
			Vector2 Xi = hammersley2d(i, SAMPLE_COUNT);

			{ // A and B
				Vector3 H = ImportanceSampleGGX(Xi, N, roughness);
				Vector3 L = (2.0 * V.dotProduct(H) * H - V).normalizedCopy();

				float NdotL = std::max(L.z, 0.f);
				float NdotV = std::max(V.z, 0.f);
				float NdotH = std::max(H.z, 0.f);
				float VdotH = std::max(V.dotProduct(H), 0.f);

				if (NdotL > 0.0)
				{
					float G = geometry_Smith(NdotV, NdotL, roughness);
					float G_Vis = (G * VdotH) / (NdotH * NdotV);
					float Fc = pow(1.0 - VdotH, 5.0);

					A += (1.0 - Fc) * G_Vis;
					B += Fc * G_Vis;
				}
			}
		}
		return Vector3(A, B, C) / float(SAMPLE_COUNT);
	}

	/* traverse all 2d coord for lut part */
	void calculate_BRDF_LUT(TGAImage& image)
	{
		int i, j;
		for (i = 0; i < 256; i++)
		{
			for (j = 0; j < 256; j++)
			{
				Vector3 color;
				if (i == 0)
					color = IntegrateBRDF(0.002f, j / 256.0f);
				else
					color = IntegrateBRDF(i / 256.0f, j / 256.0f);
				//cout << irradiance << endl;
				int red = std::min(color.x * 255.0f, 255.f);
				int green = std::min(color.y * 255.0f, 255.f);
				int blue = std::min(color.z * 255.0f, 255.f);

				//cout << irradiance << endl;
				TGAColor temp(red, green, blue);
				image.set(i, j, temp);
			}
		}
	}

	/* traverse all mipmap level for prefilter map */
	void foreach_prefilter_miplevel(TGAImage& image, Model::Ptr model)
	{
		const char* faces[6] = { "px", "nx", "py", "ny", "pz", "nz" };
		char paths[6][256];
		const int thread_num = 4;

		for (int mip_level = 8; mip_level < 10; mip_level++)
		{
			for (int j = 0; j < 6; j++) {
				sprintf_s(paths[j], "%s/m%d_%s.tga", "./obj/common2", mip_level, faces[j]);
			}
			int factor = 1;
			for (int temp = 0; temp < mip_level; temp++)
				factor *= 2;
			int w = 512 / factor;
			int h = 512 / factor;

			if (w < 64)
				w = 64;
			if (h < 64)
				h = 64;
			std::cout << w << h << std::endl;
			image = TGAImage(w, h, TGAImage::RGB);
			for (int face_id = 0; face_id < 6; face_id++)
			{
				std::thread thread[thread_num];
				for (int i = 0; i < thread_num; i++)
					thread[i] = std::thread(generate_prefilter_map, i, face_id, mip_level, model, std::ref(image));
				for (int i = 0; i < thread_num; i++)
					thread[i].join();

				//calculate_BRDF_LUT();
				image.flip_vertically(); // to place the origin in the bottom left corner of the image
				image.write_tga_file(paths[face_id]);
			}
		}

	}

	/* traverse all faces of cubemap for irradiance map */
	void foreach_irradiance_map(TGAImage& image, Model::Ptr model)
	{
		const char* faces[6] = { "px", "nx", "py", "ny", "pz", "nz" };
		char paths[6][256];
		const int thread_num = 4;


		for (int j = 0; j < 6; j++) {
			sprintf_s(paths[j], "%s/i_%s.tga", "./obj/common2", faces[j]);
		}
		image = TGAImage(256, 256, TGAImage::RGB);
		for (int face_id = 0; face_id < 6; face_id++)
		{
			std::thread thread[thread_num];
			for (int i = 0; i < thread_num; i++)
				thread[i] = std::thread(generate_irradiance_map, i, face_id, model, std::ref(image));
			for (int i = 0; i < thread_num; i++)
				thread[i].join();

			image.flip_vertically(); // to place the origin in the bottom left corner of the image
			image.write_tga_file(paths[face_id]);
		}

	}
} // OEngine