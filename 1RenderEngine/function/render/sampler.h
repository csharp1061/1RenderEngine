#pragma once

#include "../../core/math/math_headers.h"
#include "../../resource/model.h"
#include "../../resource/tgaimage.h"

#include <memory>
#include <iostream>

namespace OEngine
{
	Vector3 texture_sample(Vector2 uv, TGAImage* image);

	Vector3 cubemap_sample(Vector3 direction, cubemap_t* cubemap);

	void generate_prefilter_map(int thread_id, int face_id, int mip_level, Model::Ptr model, TGAImage& image);
	void generate_irradiance_map(int thread_id, int face_id, Model::Ptr model, TGAImage& image);
} // OEngine
