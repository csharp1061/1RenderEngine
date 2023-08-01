#include "../function/render/shader.h"

namespace OEngine
{
	// TODO: 
	static Vector3 calNormalFromNormalMap()
	{
		return Vector3::UNIT_SCALE;
	}

	void PhongShader::vertex_shader(int nfaces, int nvertex)
	{
		Vector4 temp_vertex = Vector4(m_payload.model->vert(nfaces, nvertex), 1.f);
		Vector4 temp_normal = Vector4(m_payload.model->normal(nfaces, nvertex), 1.f);

		m_payload.uv_attri[nvertex]			= m_payload.model->uv(nfaces, nvertex);
		m_payload.in_texCoords[nvertex]		= m_payload.uv_attri[nvertex];
		m_payload.clipCoord_attri[nvertex]	= m_mvp * temp_vertex;
		m_payload.in_clipPos[nvertex]		= m_payload.clipCoord_attri[nvertex];

		for (int i = 0; i < 3; i++)
		{
			m_payload.worldCoord_attri[nvertex][i] = temp_vertex[i];
			m_payload.in_worldPos[nvertex][i] = temp_vertex[i];
			m_payload.normal_attri[nvertex][i] = temp_normal[i];
			m_payload.in_normal[nvertex][i] = temp_normal[i];
		}
	}

	Vector3 PhongShader::fragment_shader(float alpha, float gamma, float beta)
	{
		// 顶点数据存储在m_payload结构体中
		

		// 如果采用的是法向量贴图
		// if (m_payload.model->normal_map)
			// 取值normal
		Vector3* worldPos	= m_payload.worldCoord_attri;
		Vector3* normals	= m_payload.normal_attri;
		Vector2* texCoords	= m_payload.uv_attri;

		Vector3 fragPos = interpolate(alpha, gamma, beta, worldPos[0], worldPos[1], worldPos[2]);
		Vector3 normal = interpolate(alpha, gamma, beta, normal[0], normal[1], normal[2]).normalizedCopy();
		Vector2 texCoord = interpolate(alpha, gamma, beta, texCoords[0], texCoords[1], texCoords[2]);

		Vector3 lightDir = (m_light.position - fragPos).normalizedCopy();
		Vector3 viewDir = (m_payload.camera->m_eye - fragPos).normalizedCopy();
		Vector3 color = m_payload.model->diffuse(texCoord);
		 
		Vector3 halfVec = (lightDir + viewDir).normalizedCopy();

		Vector3 ka{ 0.35, 0.35, 0.35 };
		Vector3 kd = color;
		// Vector3 kd{ 0.5000, 0.5000, 0.5000 };
		Vector3 ks{ 0.7937, 0.7937, 0.7937 };

		Vector3 light_ambient_intensity = kd;
		Vector3 light_diffuse_intensity = Vector3(0.9, 0.9, 0.9);
		Vector3 light_specular_intensity = Vector3(0.15, 0.15, 0.15);

		Vector3 result{ 0, 0, 0 };

		float diff = std::max(lightDir.dotProduct(normal), 0.f);
		float spec = std::pow(std::max(halfVec.dotProduct(normal), 0.f), 150);

		Vector3 diffuse = kd * light_diffuse_intensity * diff;
		Vector3 specular = ks * light_specular_intensity * spec;
		Vector3 ambient = ka * light_ambient_intensity;

		result = diffuse + specular + ambient;

		return result * 255.f;
	}
} // OEngine