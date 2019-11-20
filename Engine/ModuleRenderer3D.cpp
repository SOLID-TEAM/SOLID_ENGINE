#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleEditor.h"
#include "W_Scene.h"
#include "C_Transform.h"

#include <gl\glew.h>
#include "SDL\include\SDL_opengl.h"
#include "external/MathGeoLib/include/MathGeoLib.h"

#include "KDTree.h"
#include "DynTree.h"

#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "glu32.lib")
#pragma comment (lib, "glew32.lib")    /* link OpenGL Utility lib     */


ModuleRenderer3D::ModuleRenderer3D(bool start_enabled) : Module(start_enabled)
{
	name.assign("Renderer3D");
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init(Config& config)
{
	LOG("[Init] Creating 3D Renderer context");
	bool ret = true;
	
	// Initialize GLEW
	glewExperimental = true; // Needed for core profile

	GLenum error = glewInit();

	if (error != GLEW_OK) {
		LOG("[Error] Failed to initialize GLEW\n", glewGetErrorString(error));
		ret = false;
	}
	else
		LOG("[Init] Using Glew %s", glewGetString(GLEW_VERSION));
	
	if(ret == true)
	{
		//Use Vsync
		if(VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			LOG("[Error] Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("[Error] Initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("[Error] Initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		//glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
		
		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);
		
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].Init();
		lights[0].Active(true);
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
			
		glEnable(GL_LINE_SMOOTH);		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_MULTISAMPLE);

		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	}

	// Store version opengl/graphic drivers
	openglGDriversVersionString.assign((const char*)glGetString(GL_VERSION));

	// Store glew version
	glewVersionString.assign((const char*)glewGetString(GLEW_VERSION));

	LOG("[Info] Vendor: %s", glGetString(GL_VENDOR));
	LOG("[Info] Renderer: %s", glGetString(GL_RENDERER));
	LOG("[Info] OpenGL version supported: %s", openglGDriversVersionString.data());
	LOG("[Info] GLSL: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

	return ret;
}

update_status ModuleRenderer3D::PostUpdate(float dt)
{
	// Always swap window  ------------------------

	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}


bool ModuleRenderer3D::CleanUp()
{
	LOG("[CleanUp] Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleRenderer3D::RenderAABB(math::AABB& aabb, float width, float4& color)
{
	float3 vertices[24];
	aabb.ToEdgeList(vertices);

	uint vertex_id = 0;

	glGenBuffers(1, (GLuint*) & (vertex_id));
	glBindBuffer(GL_ARRAY_BUFFER, vertex_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24 * 3, vertices, GL_STATIC_DRAW);


	BeginDebugDraw(color);
	glLineWidth(width);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glDrawArrays(GL_LINES, 0, aabb.NumVerticesInEdgeList());
	glDisableClientState(GL_VERTEX_ARRAY);


	glLineWidth(1.f);
	EndDebugDraw();

	glDeleteBuffers(1, &vertex_id);
}

void ModuleRenderer3D::RenderOBB(math::OBB& obb, float width, float4& color)
{
	float3 vertices[24];
	obb.ToEdgeList(vertices);

	uint vertex_id = 0;

	glGenBuffers(1, (GLuint*) & (vertex_id));
	glBindBuffer(GL_ARRAY_BUFFER, vertex_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24 * 3, vertices, GL_STATIC_DRAW);


	BeginDebugDraw(color);
	glLineWidth(width);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glDrawArrays(GL_LINES, 0, obb.NumVerticesInEdgeList());
	glDisableClientState(GL_VERTEX_ARRAY);


	glLineWidth(1.f);
	EndDebugDraw();

	glDeleteBuffers(1, &vertex_id);
}

void ModuleRenderer3D::RenderFrustum(math::Frustum& frustum, float width, float4& color)
{
	float3 vertices[24];
	
	for (int i = 0; i < 12; ++i)
	{
		vertices[i * 2] = frustum.Edge(i).a;
		vertices[i * 2 + 1] = frustum.Edge(i).b;
	}

	uint vertex_id = 0;

	glGenBuffers(1, (GLuint*) & (vertex_id));
	glBindBuffer(GL_ARRAY_BUFFER, vertex_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24 * 3, vertices, GL_STATIC_DRAW);


	BeginDebugDraw(color);
	glLineWidth(width);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glDrawArrays(GL_LINES, 0, 24);
	glDisableClientState(GL_VERTEX_ARRAY);


	glLineWidth(1.f);
	EndDebugDraw();

	glDeleteBuffers(1, &vertex_id);
}

void ModuleRenderer3D::RenderKDTree(KDTree& kdtree, float width)
{
	std::stack<KDTreeNode*> nodes_stack;
	std::stack<KDTreeNode*> nodes_to_render;
	
	if (kdtree.root == nullptr)
	{
		return;
	}

	nodes_stack.push(kdtree.root);

	while (!nodes_stack.empty())
	{
		KDTreeNode* node = nodes_stack.top();

		nodes_to_render.push(node);

		nodes_stack.pop();

		if (node->is_leaf == false)
		{
			if (node->left_child->bucket_members > 0) nodes_stack.push(node->left_child);
			if (node->right_child->bucket_members > 0) nodes_stack.push(node->right_child);
		}
	}
	
	glDepthFunc(GL_ALWAYS);

	while (!nodes_to_render.empty())
	{
		KDTreeNode* node = nodes_to_render.top();
		float4 color;

		if (node != kdtree.root)
		{
			switch (uint dimension = node->depth % 3)
			{
			case 0:
				color = { 1.f, 0.f, 0.f, 1.f };
				break;
			case 1:
				color = { 0.f, 1.f, 0.f, 1.f };
				break;
			case 2:
				color = { 0.f, 0.f, 1.f, 1.f };
				break;
			}


			RenderAABB(node->aabb, width - (float)node->depth * 0.3f, color);
		}
		else
		{
			color = { 1.f, 1.f, 1.f, 1.f };
			RenderAABB(node->aabb, width, color);
		}

		

		nodes_to_render.pop();
	}
	
	glDepthFunc(GL_LESS);
}

void ModuleRenderer3D::RenderDynTree(DynTree& kdtree, float width, float4& color)
{
	std::stack<DynTreeNode*> nodes_stack;

	nodes_stack.push(kdtree.root);

	while (!nodes_stack.empty())
	{
		DynTreeNode* node = nodes_stack.top();

		RenderAABB(node->aabb , width, color);

		nodes_stack.pop();

		if (node->node_left != nullptr)
			nodes_stack.push(node->node_left);
		if (node->node_right != nullptr)
			nodes_stack.push(node->node_right);
	}
}

void ModuleRenderer3D::RenderCircle(float3 position, float radio, int num_segments)
{
	glBegin(GL_LINE_LOOP);

	for (int i = 0; i < num_segments; ++i) 
	{
		float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);
		float x = radio * cosf(theta);
		float y = radio * sinf(theta);
		glVertex3f(x + position.x, y + position.y, position.z);
	}

	glEnd();
}

void ModuleRenderer3D::BeginDebugDraw(float4& color)
{
	glDisable(GL_LIGHTING);
	glColor4fv(&color[0]);
}

void ModuleRenderer3D::EndDebugDraw()
{
	GLfloat color_default[] = { 1.f, 1.f, 1.f, 1.f };
	glEnable(GL_LIGHTING);
	glColor4fv(color_default);
}

void ModuleRenderer3D::SetDefaultColorMaterial()
{
	glColor4fv((float*)&render_config.default_color_mat);
}

RenderConfig& ModuleRenderer3D::GetRenderConfig()
{
	return render_config;
}

std::string ModuleRenderer3D::GetGlewVersionString() const
{
	return glewVersionString;
}

bool ModuleRenderer3D::Save(Config& config)
{
	return true;
}

void ModuleRenderer3D::Load(Config& config)
{

}

void DebugRender::Render()
{
	if (type_name == typeid(AABB).name())			App->renderer3D->RenderAABB((AABB&)obj_to_render, width, color);
	else if (type_name == typeid(OBB).name())		App->renderer3D->RenderOBB((OBB&)obj_to_render, width, color);
	else if (type_name == typeid(Frustum).name())	App->renderer3D->RenderFrustum((Frustum&)obj_to_render, width, color);
}