#pragma once

#include "../../core/math/math_headers.h"
#include "../../resource/tgaimage.h"

namespace OEngine
{
	typedef struct cubemap
	{
		TGAImage* faces[6];
	} cubemap_t;

	Vector3 texture_sample(Vector2 uv, TGAImage* image);

	Vector3 cubemap_sample(Vector3 direction, cubemap_t* cubemap);
} // OEngine
