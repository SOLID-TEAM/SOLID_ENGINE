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

void ModuleRenderer3D::RenderTest()
{

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


FBO::FBO()
{
	for (int i = 0; i < 8; ++i)
	{
		ID[i] = 0;
	}

	GenerateFBO();
}

FBO::~FBO()
{
	DeleteFBO();
}

void FBO::BeginFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, ID[MULTISAMPLING_FBO]);
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void FBO::EndFBO()
{
	// Blit Frame buffer -------------------------------------------------------

	glBindFramebuffer(GL_READ_FRAMEBUFFER, ID[MULTISAMPLING_FBO]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ID[NORMAL_FBO]);

	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	// Generate Mipmap --------------------------------------------------------

	glBindTexture(GL_TEXTURE_2D, ID[NORMAL_TEXTURE]);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Copy depht component ---------------------------------------------------

	//glBindTexture(GL_TEXTURE_2D, ID[DEPTH_TEAXTURE]);
	//glReadBuffer(GL_BACK); // Ensure we are reading from the back buffer.
	//glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 0, 0, width, height, 0);

}

void FBO::UpdateFBO( float width, float height)
{
	this->width = width;
	this->height = height;
	bool fboUsed = true;
	// Normal =====================================================================

	// Texture ---------------------------------------------

	glBindTexture(GL_TEXTURE_2D, ID[NORMAL_TEXTURE]);

	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	// Depth & Stencil -------------------------------------

	glBindRenderbuffer(GL_RENDERBUFFER, ID[NORMAL_DEPTH_RBO]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Frame -----------------------------------------------

	glBindFramebuffer(GL_FRAMEBUFFER, ID[NORMAL_FBO]);

	// Attachment ------------------------------------------

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ID[NORMAL_TEXTURE], 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ID[NORMAL_DEPTH_RBO]);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) 
		fboUsed = false;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Multisample ================================================================

	// Color -----------------------------------------------

	glBindRenderbuffer(GL_RENDERBUFFER, ID[MULTISAMPLING_COLOR_RBO]);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaa, GL_RGBA8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Depth & Stencil -------------------------------------

	glBindRenderbuffer(GL_RENDERBUFFER, ID[MULTISAMPLING_DEPTH_RBO]);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaa, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);


	// Frame -----------------------------------------------

	glBindFramebuffer(GL_FRAMEBUFFER, ID[MULTISAMPLING_FBO]);

	// Attachment ------------------------------------------

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, ID[MULTISAMPLING_COLOR_RBO]);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ID[MULTISAMPLING_DEPTH_RBO]);


	GLenum status_2 = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status_2 != GL_FRAMEBUFFER_COMPLETE)
		fboUsed *= false;
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

//if (z_buffer_mode)
//{
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

//	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_FLOAT, 0);
//}
//else

void FBO::GenerateFBO()
{
	glGenTextures(1,		&ID[NORMAL_TEXTURE]);
	glGenTextures(1,		&ID[DEPTH_TEAXTURE]);

	glGenFramebuffers(1,	&ID[NORMAL_FBO]);
	glGenFramebuffers(1,	&ID[MULTISAMPLING_FBO]);

	glGenRenderbuffers(1,	&ID[NORMAL_DEPTH_RBO]);
	glGenRenderbuffers(1,	&ID[MULTISAMPLING_COLOR_RBO]);
	glGenRenderbuffers(1,	&ID[MULTISAMPLING_DEPTH_RBO]);

}

void FBO::DeleteFBO()
{
	glDeleteTextures(1,		&ID[NORMAL_TEXTURE]);
	glDeleteTextures(1,		&ID[DEPTH_TEAXTURE]);

	glDeleteFramebuffers(1, &ID[NORMAL_FBO]);
	glDeleteFramebuffers(1, &ID[MULTISAMPLING_FBO]);

	glDeleteRenderbuffers(1, &ID[NORMAL_DEPTH_RBO]);
	glDeleteRenderbuffers(1, &ID[MULTISAMPLING_COLOR_RBO]);
	glDeleteRenderbuffers(1, &ID[MULTISAMPLING_DEPTH_RBO]);
}

uint FBO::GetFBOTexture()
{
	return ID[NORMAL_TEXTURE];
}




