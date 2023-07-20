#pragma once

#include "../../resource/texture.h"
#include "../../resource/tgaimage.h"

namespace OEngine
{
	typedef struct cubemap 
	{
		TGAImage* faces[6];
	} cubemap_t;

	struct fragment_shader_payload
	{
		fragment_shader_payload()
		{
			texture = nullptr;
		}

		fragment_shader_payload(const Vector3& col, const Vector3& nor, const Vector2& uv, Texture* tex)
			: color(col), normal(nor), tex_coords(uv), texture(tex) {}

		Vector3 view_pos;
		Vector3 color;
		Vector3 normal;
		Vector2 tex_coords;
		Texture* texture;
	};

	struct vertex_shader_payload
	{
		Vector3 position;
	};
} // OEngine