#pragma once
#include "Module.h"
#include "Globals.h"
#include "Light.h"
#include <string>

#include "external/MathGeoLib/include/Math/float4.h"

#define MAX_LIGHTS 8

struct RenderConfig
{
	bool gl_depth_test = true;
	bool gl_cull_face = true;
	bool gl_lighting = true;
	bool gl_color_material = true;
	bool wireframe = true;

	float max_line_w = 10.0f;
	float min_line_w = 0.0f;
	float max_point_size = 10.0f;
	float min_point_size = 0.0f;
	float max_n_length = 3.0f;
	float min_n_length = 0.0f;
	float min_alpha = 0.f;
	float max_alpha = 1.f;

	ImVec4 default_color_mat = { 1.0f,1.0f,1.0f,1.0f };
};

class FBO
{
public:

	enum FBORenderTarget
	{
		NORMAL_FBO,
		NORMAL_TEXTURE,
		NORMAL_DEPTH_RBO,
		MULTISAMPLING_FBO,
		MULTISAMPLING_COLOR_RBO,
		MULTISAMPLING_DEPTH_RBO,
	};

	FBO();

	~FBO();

	void BeginFBO();

	void EndFBO();

	void GenerateFBO();

	void UpdateFBO(float width, float height);

	void DeleteFBO();

	uint GetRenderTexture();

public:

	uint samples = 4;
	uint ID[8];

	float4 clear_color = { 0.1, 0.1, 0.1, 1.f };
	float width = 0, height = 0;

};

class ModuleRenderer3D : public Module
{
public:

	ModuleRenderer3D(bool start_enabled = true);

	~ModuleRenderer3D();

	bool Init(Config& config);

	update_status PreUpdate(float dt);

	update_status Update(float dt);

	update_status PostUpdate(float dt);

	update_status Draw();

	bool CleanUp();

	bool Save(Config& config);

	void Load(Config& config);

	void OnResize();

	std::string GetGlewVersionString() const;

	RenderConfig& GetRenderConfig();

	void SetDefaultColorMaterial();

public:
	
	RenderConfig render_config;
	Light lights[MAX_LIGHTS];
	SDL_GLContext context;

	mat3x3 normal_mat;
	mat4x4 model_mat, view_mat, projection_mat;

	FBO scene_fbo;

private:

	bool on_resize = true;
	std::string openglGDriversVersionString;
	std::string glewVersionString;
};
