#pragma once

#include "../render/rasterizer.h"
#include "../../core/base/timer.h"
#include "../platform/camera.h"

#include <opencv2/opencv.hpp>

#include <memory>
#include <iostream>
#include <string>
#include <vector>
#include <functional>

namespace OEngine
{
	class Scene
	{
	private:
		// typedef std::function<void(int)> KeyBoardCB;

		std::string m_name;
		int m_width, m_height;
		cv::Mat m_image;
		int FPS; // 限制scene的展示帧数 以减轻计算负担
		Rasterizer::Ptr m_renderer = nullptr;

	public:
		Scene(const std::string& name, int fps, Rasterizer::Ptr ptr);

		void update();

		/*void processInput(int key) { KeyBoardCB(key); }*/
	};

} //  OEngine