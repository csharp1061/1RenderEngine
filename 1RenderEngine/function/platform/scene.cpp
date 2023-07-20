#include "scene.h"

namespace OEngine
{
	Scene::Scene(const std::string& name, int fps, Rasterizer::Ptr r) : FPS(fps), m_renderer(r)
	{
		m_width = r->m_width;
		m_height = r->m_height;
		m_name = name;

		cv::namedWindow(m_name, cv::WINDOW_NORMAL);
		cv::resizeWindow(m_name, m_width, m_height);
	}

	void Scene::update()
	{
		m_image = cv::Mat(m_width, m_height, CV_32FC3, m_renderer->frame_buffer().data());
		// std::cout << m_image.size() << '\n';
		m_image.convertTo(m_image, CV_8UC3, 1.0f);
		cv::cvtColor(m_image, m_image, cv::COLOR_RGB2BGR);

		cv::imshow(m_name, m_image);
	}
} // OEngine