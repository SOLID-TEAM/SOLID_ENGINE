#pragma once
#include "Module.h"
#include "Globals.h"
#include "Light.h"
#include <string>

#include "external/MathGeoLib/include/Math/float4.h"

#define MAX_LIGHTS 8

class FBO
{
public:

	enum BufferType
	{
		DEPTH_TEAXTURE,
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

	uint GetFBOTexture();

	// Only multiple of 2 values // 0 : MSAA disabled // 2-16 : MSAA enabled
	void SetMSAA( uint MSAA) 
	{
		if (MSAA % 2 != 0 || MSAA > 0 || MSAA <= 16)
		{
			msaa = MSAA;
		}
	}

	uint GetMSAA()
	{
		return msaa;
	}

public:

	float4 clear_color = { 0.1, 0.1, 0.1, 1.f };

private:
	bool z_buffer_mode = true;
	uint ID[8];
	uint msaa = 4;
	float width = 0, height = 0;
};

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

typedef void*  SDL_GLContext;
class KDTree;
class DynTree;

class ModuleRenderer3D : public Module
{
public:

	ModuleRenderer3D(bool start_enabled = true);

	~ModuleRenderer3D();

	bool Init(Config& config);

	update_status PostUpdate(float dt);

	bool CleanUp();

	// Render Functions ------------------------------------------------------------

	void RenderAABB(math::AABB& aabb, float width, float4& color);

	void RenderOBB(math::OBB& obb, float width, float4& color);

	void RenderFrustum(math::Frustum& frustum, float width, float4& color);

	void RenderKDTree(KDTree& kdtree, float width);

	void RenderDynTree(DynTree& kdtree, float width, float4& color);

	void RenderCircle(float3 postion, float r, int num_segments);

	static void BeginDebugDraw(float4& color);

	static void EndDebugDraw();


	bool Save(Config& config);

	void Load(Config& config);

	std::string GetGlewVersionString() const;

	RenderConfig& GetRenderConfig();

	void SetDefaultColorMaterial();



public:
	
	RenderConfig render_config;
	Light lights[MAX_LIGHTS];
	SDL_GLContext context;

private:

	std::string openglGDriversVersionString;
	std::string glewVersionString;
};
