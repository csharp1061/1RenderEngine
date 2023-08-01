#include <iostream>

#include "core/math/math_headers.h"
#include "function/render/rasterizer.h"
#include "function/render/light.h"
#include "resource/OBJ_Loader.h"
#include "resource/model.h"
#include "function/platform/scene.h"
#include "function/platform/camera.h"
#include "./core/base/timer.h"
#include "function/platform/win32.h"

const std::string NAME = "OEngine";
float deltatime = 0;

const unsigned int M_WIDTH = 800;
const unsigned int M_HEIGHT = 600;

const OEngine::Vector3 EYE{ 0, 1, 5 };
const OEngine::Vector3 UP{ 0, 1, 0 };
const OEngine::Vector3 TARGET{ 0, 1, 0 };

auto EUT_CAMERA = std::make_shared<OEngine::Camera>(EYE, TARGET, UP, (float)M_WIDTH/M_HEIGHT);

void updateMatrix(OEngine::Camera::Ptr camera, OEngine::Matrix4x4 view_mat, OEngine::Matrix4x4 perspective_mat
	, OEngine::ShaderProgram::Ptr skyboxShader, OEngine::ShaderProgram::Ptr shader);

int main()
{
	float angle = 140.0;
	OEngine::Vector3 eye_pos{ 0, 0, 1 };
	OEngine::Vector3 yaxis{ 0, 1, 0 };
	OEngine::Vector3 front{ 0, 0,-1 };
	OEngine::Radian radian(angle);
	OEngine::Quaternion quater(radian, yaxis);

	OEngine::window_init(M_WIDTH, M_HEIGHT, "OERender");

	auto m = std::make_shared<OEngine::Model>("./models/gun/Cerberus.obj");
	auto skyBox = std::make_shared<OEngine::Model>("./models/skybox2/box.obj", 1);

	auto r = std::make_shared<OEngine::Rasterizer>(M_WIDTH, M_HEIGHT);
	// OEngine::Scene scene(NAME, 30, r);

	/*
	*  可编程渲染管线
	*		vertex_shader	:  顶点着色器
	*			   m, v, p	:  模型，观察，投影变换
	*		fragment_shader :  片段着色器
	* 
	*		
	*/
	OEngine::Matrix4x4 model		=  OEngine::Matrix4x4::IDENTITY;
	OEngine::Matrix4x4 view			=  OEngine::Matrix4x4::IDENTITY;
	OEngine::Matrix4x4 projection	=  OEngine::Matrix4x4::IDENTITY;
	
	// model part
	view = OEngine::Math::makeLookAtMatrix(EUT_CAMERA->m_eye, EUT_CAMERA->m_target, EUT_CAMERA->m_up);
	projection = OEngine::Math::makePerspectiveMatrix(OEngine::Radian(45.f), 1, -0.1, -100);

	r->set_model(model);
	OEngine::Timer timer(true);

	auto shader		  = std::make_shared<OEngine::PhongShader>();
	auto skyboxShader = std::make_shared<OEngine::SkyBoxShader>();
	auto PBRShader	  = std::make_shared<OEngine::PBRShader>();

	shader->set_model(model);
	shader->m_payload.model = m;
	shader->m_payload.camera = EUT_CAMERA;
	shader->m_light.position = OEngine::Vector3(0, 0, 1);
	shader->m_light.intensity = OEngine::Vector3(500, 500, 500);

	skyboxShader->m_payload.model = skyBox;
	skyboxShader->m_payload.camera = EUT_CAMERA;

	PBRShader->set_model(model);
	PBRShader->m_payload.model = m;
	PBRShader->m_payload.camera = EUT_CAMERA;

	while (!OEngine::window->is_close)
	{
		auto delta = timer.duration();
		deltatime = delta;

		// 将数据绘制进 framebuffer 中
		r->clear(OEngine::Buffers::Color | OEngine::Buffers::Depth);
		
		OEngine::handle_events(EUT_CAMERA);

		updateMatrix(EUT_CAMERA, view, projection, skyboxShader, PBRShader);

		// r->draw(m);
		r->draw(skyBox, skyboxShader);
		// r->draw(m, shader);
		r->draw(m, PBRShader);

		OEngine::window->mouse_info.wheel_delta = 0;
		OEngine::window->mouse_info.orbit_delta = OEngine::Vector2(0, 0);
		OEngine::window->mouse_info.fv_delta	= OEngine::Vector2(0, 0);

		OEngine::window_draw(r->frame_buffer());
		OEngine::msg_dispatch();
	}

	OEngine::window_destroy();

	// system("pause");
	return 0;
}

void updateMatrix(OEngine::Camera::Ptr camera, OEngine::Matrix4x4 view_mat, OEngine::Matrix4x4 perspective_mat
	, OEngine::ShaderProgram::Ptr skyboxShader, OEngine::ShaderProgram::Ptr shader)
{
	view_mat = OEngine::Math::makeLookAtMatrix(camera->m_eye, camera->m_target, camera->m_up);
	OEngine::Matrix4x4 mvp = perspective_mat * view_mat;

	shader->m_view = view_mat;
	shader->m_mvp  = mvp;

	if (skyboxShader != NULL)
	{
		OEngine::Matrix4x4 viewSky = view_mat;
		viewSky[0][3] = 0;
		viewSky[1][3] = 0;
		viewSky[2][3] = 0;
		skyboxShader->m_view = viewSky;
		skyboxShader->m_mvp = perspective_mat * viewSky;
	}
}