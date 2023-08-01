#include "../function/render/shader.h"

namespace OEngine
{
	void SkyBoxShader::vertex_shader(int nfaces, int nvertex)
	{
		int i = 0;
		Vector3 temp = m_payload.model->vert(nfaces, nvertex);
		Vector4 temp_vert = Vector4(temp);
		Vector4 temp_norm = Vector4(m_payload.model->normal(nfaces, nvertex));

		m_payload.uv_attri[nvertex] = m_payload.model->uv(nfaces, nvertex);
		m_payload.clipCoord_attri[nvertex] = m_mvp * temp_vert;	

		for (int i = 0; i < 3; i++)
		{
			m_payload.normal_attri[nvertex][i]		= temp_norm[i];
			m_payload.worldCoord_attri[nvertex][i]	= temp_vert[i];
		}
	}

	Vector3 SkyBoxShader::fragment_shader(float alpha, float gamma, float beta)
	{
		Vector3 result;
		Vector4* windowPoses = m_payload.clipCoord_attri;
		Vector3* worldPoses = m_payload.worldCoord_attri;

		float Z = 1.0 / (alpha / windowPoses[0].w + gamma / windowPoses[1].w + beta / windowPoses[2].w);
		Vector3 worldPos = (alpha * worldPoses[0] / windowPoses[0].w + gamma * worldPoses[1] / windowPoses[1].w +
			beta * worldPoses[2] / windowPoses[2].w) * Z;

		result = cubemap_sample(worldPos, m_payload.model->environment_map);

		return result * 255.f;
	}
} // OEngine