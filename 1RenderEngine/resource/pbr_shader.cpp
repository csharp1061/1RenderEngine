#include "../function/render/shader.h"
#include "../function/render/sampler.h"

namespace OEngine
{
	static Vector3 FresnelSchlick(float cosTheta, const Vector3& F0)
	{
		return F0 + (1 - F0) * std::pow(Math::clamp((1 - cosTheta), 0.0, 1.0), 5.0);
	}

	// ACES色彩映射
	static float FloatAces(float value)
	{
		float a = 2.51f;
		float b = 0.03f;
		float c = 2.43f;
		float d = 0.59f;
		float e = 0.14f;
		value = (value * (a * value + b)) / (value * (c * value + d) + e);
		return Math::clamp(value, 0, 1);
	}

	static float DistributionGGX(const Vector3& n, const Vector3& h, float roughness)
	{
		float a = roughness * roughness;
		float a2 = a * a;
		float NdotH = std::max(n.dotProduct(h), 0.f);
		float NdotH2 = NdotH * NdotH;

		float num = a2;
		float denom = NdotH2 * (a2 - 1.f) + 1.f;
		denom = Math_PI * denom * denom;

		return num / denom;
	}

	static float GeometrySchlickGGX(float dotV, float roughness)
	{
		float a = roughness + 1.0;
		float k = (a * a) / 8.0;

		float num = dotV;
		float denom = num * (1.0 - k) + k;

		return num / denom;
	}

	static float GeometrySmith(const Vector3& n, const Vector3& v, const Vector3& l, float roughness)
	{
		float ndotv = std::max(n.dotProduct(v), 0.f);
		float ndotl = std::max(n.dotProduct(l), 0.f);

		float ggx1 = GeometrySchlickGGX(ndotv, roughness);
		float ggx2 = GeometrySchlickGGX(ndotl, roughness);

		return ggx1 * ggx2;
	}

	static Vector3 GetNormalFromMap(Vector3& normal, Vector3* worldPos, Vector2* uvs, const Vector2& uv, TGAImage* normal_map)
	{
		float x1 = uvs[1][0] - uvs[0][0];
		float y1 = uvs[1][1] - uvs[0][1];
		float x2 = uvs[2][0] - uvs[0][0];
		float y2 = uvs[2][1] - uvs[0][1];
		float det = (x1 * y2 - x2 * y1);

		Vector3 e1 = worldPos[1] - worldPos[0];
		Vector3 e2 = worldPos[2] - worldPos[0];

		Vector3 t = e1 * y2 + e2 * (-y1);
		Vector3 b = e1 * (-x2) + e2 * x1;
		t /= det;
		b /= det;

		normal.normalise();
		t = (t - t.dotProduct(normal) * normal).normalizedCopy();
		b = (b - b.dotProduct(normal) * normal - b.dotProduct(t) * t).normalizedCopy();

		Vector3 sample = texture_sample(uv, normal_map);
		sample = Vector3(sample[0] * 2 - 1, sample[1] * 2 - 1, sample[2] * 2 - 1);

		Vector3 normal_new = t * sample[0] + b * sample[1] + normal * sample[2];
		return normal_new;
	}

	Vector3 ReinhardMapping(Vector3& color)
	{
		for (int i = 0; i < 3; i++)
		{
			color[i] = FloatAces(color[i]);
			color[i] = std::pow(color[i], 1.0 / 2.2);
		}
		return color;
	}

	void PBRShader::vertex_shader(int nfaces, int nvertex)
	{
		Vector4 temp_vertex = Vector4(m_payload.model->vert(nfaces, nvertex), 1.f);
		Vector4 temp_normal = Vector4(m_payload.model->normal(nfaces, nvertex), 1.f);

		m_payload.uv_attri[nvertex] = m_payload.model->uv(nfaces, nvertex);
		m_payload.in_texCoords[nvertex] = m_payload.uv_attri[nvertex];
		m_payload.clipCoord_attri[nvertex] = m_mvp * temp_vertex;
		m_payload.in_clipPos[nvertex] = m_payload.clipCoord_attri[nvertex];

		for (int i = 0; i < 3; i++)
		{
			m_payload.worldCoord_attri[nvertex][i] = temp_vertex[i];
			m_payload.in_worldPos[nvertex][i] = temp_vertex[i];
			m_payload.normal_attri[nvertex][i] = temp_normal[i];
			m_payload.in_normal[nvertex][i] = temp_normal[i];
		}
	}

	Vector3 PBRShader::fragment_shader(float alpha, float gamma, float beta)
	{
		Vector4* windowPos = m_payload.clipCoord_attri;
		Vector3* worldPoses = m_payload.worldCoord_attri;
		Vector3* normals = m_payload.normal_attri;
		Vector2* uvs = m_payload.uv_attri;

		float Z = 1.0 / (alpha / windowPos[0].w + gamma / windowPos[1].w + beta / windowPos[2].w);
		Vector3 normal = (alpha * normals[0] / windowPos[0].w + gamma * normals[1] / windowPos[1].w
			+ beta * normals[2] / windowPos[2].w) * Z;
		Vector2 uv = (alpha * uvs[0] / windowPos[0].w + gamma * uvs[1] / windowPos[1].w
			+ beta * uvs[2] / windowPos[2].w) * Z;
		Vector3 worldPos = (alpha * worldPoses[0] / windowPos[0].w + gamma * worldPoses[1] / windowPos[1].w
			+ beta * worldPoses[2] / windowPos[2].w) * Z;

		if (m_payload.model && m_payload.model->normal_map)
			normal = GetNormalFromMap(normal, worldPoses, uvs, uv, m_payload.model->normal_map);

		Vector3 n = normal.normalizedCopy();
		Vector3 v = (m_payload.camera->m_eye - worldPos).normalizedCopy();
		float NdotV = std::fmaxf(n.dotProduct(v), 0.f);

		float roughness = m_payload.model->roughness(uv);
		float metalness = m_payload.model->metalness(uv);
		float occlusion = m_payload.model->occlusion(uv);

		Vector3 albedo = m_payload.model->diffuse(uv);

		Vector3 color{ 0.f, 0.f, 0.f };
		Vector3 lo{ 0.f, 0.f, 0.f };
		if (NdotV > 0)
		{
			Vector3 l = (m_light.position - worldPos).normalizedCopy();
			Vector3 h = (l + v).normalizedCopy();

			/* DFG */

			// F
			Vector3 F0 = Vector3(0.04f);
			F0 = Vector3::lerp(albedo, metalness, F0[0]);
			Vector3 F = FresnelSchlick(std::max(h.dotProduct(v), 0.f), roughness);

			// NDF
			float D = DistributionGGX(n, h, roughness);

			// G
			float G = GeometrySmith(n, v, l, roughness);

			Vector3 nominator = D * F * G;
			float denominator = 4.0 * std::max(n.dotProduct(l), 0.f) * std::max(n.dotProduct(v), 0.f) + 0.001;

			Vector3 specular = nominator / denominator;

			Vector3 kS = F;
			Vector3 kD = Vector3(1.f) - kS;
			kD *= (1.0 - metalness);

			float NdotL = std::max(n.dotProduct(l), 0.f);
			lo += (kD * albedo / Math_PI + kS * specular) * NdotL;
		}
		// 环境光遮蔽
		Vector3 ambient = Vector3(0.03f) * albedo * occlusion;
		color = ambient + lo;

		color = ReinhardMapping(color);

		return color * 255.f;
		// return { alpha * 255, gamma * 255, beta * 255 };
	}
} // OEngine
