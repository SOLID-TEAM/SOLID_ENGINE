#pragma once
#include "Module.h"
#include "Globals.h"
#include "Light.h"
#include <string>
#include <typeinfo>

#include "external/MathGeoLib/include/Math/float4.h"
#include "external/MathGeoLib/include/Geometry/AABB.h"
#include "external/MathGeoLib/include/Geometry/OBB.h"
#include "external/MathGeoLib/include/Geometry/Frustum.h"


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

struct DebugRender
{
public:

	template<class T>
	void Set(T* obj_to_render, float4 color, float width )
	{
		if (typeid(T) == typeid(AABB) || typeid(T) == typeid(OBB) || typeid(T) == typeid(Frustum) == true)
		{
			this->obj_to_render = (void*)obj_to_render;
			this->color = color;
			this->width = width;
			type_name = typeid(T).name();
		} 
		else
		{
			LOG("[Error] RenderDebug(). Type is not alowed");
		}
	}

	void Render();

	void* obj_to_render = nullptr;
	float4 color;
	float width = 0.f;
	std::string type_name;
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

	update_status PostUpdate();

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
