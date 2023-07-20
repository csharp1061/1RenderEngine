#include <iostream>
#include <opencv2/opencv.hpp>

#include "core/math/math_headers.h"
#include "function/render/rasterizer.h"
#include "function/render/light.h"
#include "resource/OBJ_Loader.h"
#include "resource/model.h"
#include "function/platform/scene.h"
#include "function/platform/camera.h"
#include "./core/base/timer.h"

OEngine::Vector3 vertex_shader(const OEngine::vertex_shader_payload& payload)
{
	return payload.position;
}

OEngine::Vector3 normal_fragment_shader(const OEngine::fragment_shader_payload& payload)
{
	OEngine::Vector3 norm = (OEngine::Vector3(payload.normal.x, payload.normal.y, payload.normal.z).normalizedCopy() + OEngine::Vector3(1.f)) / 2.f;
	OEngine::Vector3 result;
	result.x = norm.x * 255.f;
	result.y = norm.y * 255.f;
	result.z = norm.z * 255.f;
	return result;
}

OEngine::Vector3 texture_fragment_shader(const OEngine::fragment_shader_payload& payload)
{
	return OEngine::Vector3();
}

OEngine::Vector3 phong_fragment_shader(const OEngine::fragment_shader_payload& payload)
{
	OEngine::Vector3 ka{ 0.005, 0.005, 0.005 };
	OEngine::Vector3 kd = payload.color;
	OEngine::Vector3 ks{ 0.7937, 0.7937, 0.7937 };

	OEngine::Vector3 color = { 0, 0, 0 };

	auto light1		 = OEngine::Light();
	light1.position  = OEngine::Vector3(20, 20, 20);
	light1.intensity = OEngine::Vector3(500, 500, 500);

	OEngine::Vector3 lightDir = light1.position - payload.view_pos;
	OEngine::Vector3 eyePos{ 0, 0, 10 };
	OEngine::Vector3 viewDir = eyePos - payload.view_pos;
	OEngine::Vector3 Normal = payload.normal.normalizedCopy();

	float diff = std::max(lightDir.dotProduct(Normal), 0.f);
	color += diff * kd;

	OEngine::Vector3 half = (lightDir + viewDir).normalizedCopy();
	float spec = std::pow(std::max(half.dotProduct(Normal), 0.f), 150);
	color += spec * ks * kd;

	color += ka * kd;

	return color;
}

const std::string NAME = "OEngine";
float deltatime = 0;

OEngine::Camera MAIN_CAMERA = OEngine::Camera(OEngine::Vector3(0, 0, 10));

void processKeyBoardInput(int key);

int main()
{
	float angle = 140.0;
	OEngine::Vector3 eye_pos{ 0, 0, 10 };
	OEngine::Vector3 yaxis{ 0, 1, 0 };
	OEngine::Vector3 front{ 0, 0,-1 };
	OEngine::Radian radian(angle);
	OEngine::Quaternion quater(radian, yaxis);

	std::string obj_path = "./models/spot/";

	OEngine::Model m("./models/spot/spot_triangulated_good.obj");

	auto r = std::make_shared<OEngine::Rasterizer>(700, 700);
	OEngine::Scene scene(NAME, 30, r);

	auto texture_path = "hmap.jpg";
	r->set_texture(OEngine::Texture(obj_path + texture_path));

	std::function<OEngine::Vector3(OEngine::fragment_shader_payload)> active_shader = normal_fragment_shader;
	
	r->set_vertex_shader(vertex_shader);
	r->set_fragment_shader(active_shader);

	/*
	*  可编程渲染管线
	*		vertex_shader	:  顶点着色器
	*			   m, v, p	:  模型，观察，投影变换
	*		fragment_shader :  片段着色器
	* 
	*				使用 std::function
	*		
	*/
	OEngine::Matrix4x4 model		=  OEngine::Matrix4x4::IDENTITY;
	OEngine::Matrix4x4 view			=  OEngine::Matrix4x4::IDENTITY;
	OEngine::Matrix4x4 projection	=  OEngine::Matrix4x4::IDENTITY;
	
	// model part
	model.makeTransform(OEngine::Vector3(1, 1, 1), OEngine::Vector3(1, 1, 1), quater);
	r->set_model(model);
	OEngine::Timer timer(true);

	while (true)
	{
		auto delta = timer.duration();
		deltatime = delta;
		view = MAIN_CAMERA.getViewMatrix();
		projection = OEngine::Math::makePerspectiveMatrix(OEngine::Radian(MAIN_CAMERA.m_zoom), 1, 0.1, 50);
		// 将数据绘制进 framebuffer 中
		r->clear(OEngine::Buffers::Color | OEngine::Buffers::Depth);
		
		r->set_view(view);
		r->set_projection(projection);


		r->draw(m);
			scene.update();
			int key = cv::waitKey(1);
			
			processKeyBoardInput(key);

			if (key == 27)
			{
				cv::destroyAllWindows();
				break;
			}	
	}
	return 0;
}

void processKeyBoardInput(int key)
{
	switch (key)
	{
	case 'w':
		MAIN_CAMERA.processKeyboard(OEngine::FORWARD, deltatime);
		break;
	case 's':
		MAIN_CAMERA.processKeyboard(OEngine::BACKWARD, deltatime);
		break;
	case 'a':
		MAIN_CAMERA.processKeyboard(OEngine::RIGHT, deltatime);
		break;
	case 'd':
		MAIN_CAMERA.processKeyboard(OEngine::LEFT, deltatime);
		break;
	default:
		break;
	}
}