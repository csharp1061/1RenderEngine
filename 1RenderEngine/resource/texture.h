#pragma once

#include "../core/math/math_headers.h"

#include <opencv2/opencv.hpp>

namespace OEngine
{
	class Texture
	{
	private:
		cv::Mat image_data;

	public:
		int width, height;

		Texture(const std::string& name)
		{
			image_data = cv::imread(name);
			cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
			width = image_data.cols;
			height = image_data.rows;
		}

		Vector3 getColor(float u, float v)
		{
			if (u < 0) u = 0;
			if (u > 1) u = 1;
			if (v < 0) v = 0;
			if (v > 1) v = 1;
			auto u_img = u * width;
			auto v_img = (1 - v) * height;
			auto color = image_data.at<cv::Vec3b>(v_img, u_img);
			return Vector3(color[0], color[1], color[2]);
		}
	};
} // OEngine