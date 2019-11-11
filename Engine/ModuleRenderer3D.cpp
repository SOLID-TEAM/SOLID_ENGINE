#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleEditor.h"
#include "W_Scene.h"

#include <gl\glew.h>
#include "SDL\include\SDL_opengl.h"

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
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
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
		glEnable(GL_BLEND);
		glEnable(GL_MULTISAMPLE);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

		glShadeModel(GL_SMOOTH);                    // shading mathod: GL_SMOOTH or GL_FLAT
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);      // 4-byte pixel alignment

		glClearColor(0, 0, 0, 0);                   // background color
		glClearStencil(0);                          // clear stencil buffer
		glClearDepth(1.0f);                         // 0 is near, 1 is far
		glDepthFunc(GL_LEQUAL);
	}

	// Store version opengl/graphic drivers
	openglGDriversVersionString.assign((const char*)glGetString(GL_VERSION));

	// Store glew version
	glewVersionString.assign((const char*)glewGetString(GLEW_VERSION));

	LOG("[Info] Vendor: %s", glGetString(GL_VENDOR));
	LOG("[Info] Renderer: %s", glGetString(GL_RENDERER));
	LOG("[Info] OpenGL version supported: %s", openglGDriversVersionString.data());
	LOG("[Info] GLSL: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

	// Generate Scene Buffers -------------------------------
	scene_fbo.GenerateFBO();

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());


	// TODO: re-added lights until we create component light, remove from here when done
	// light 0 on cam pos
	lights[0].SetPos(App->camera->position.x, App->camera->position.y, App->camera->position.z);

	for (uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();


	return UPDATE_CONTINUE;
}

update_status ModuleRenderer3D::Update(float dt)
{
	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{

	SDL_GL_SwapWindow(App->window->window);

	// prepare for main loop Draw -----------------------------------------------

	if (on_resize)
	{
		ImVec2 size = App->editor->w_scene->GetViewportSize();

		glViewport(0, 0, size.x, size.y);

		glMatrixMode(GL_PROJECTION);
		projection_mat = perspective(60.0f, size.x / size.y, 0.125f, 512.0f);
		glLoadMatrixf(&projection_mat);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		scene_fbo.UpdateFBO(size.x, size.y);

		on_resize = false;
	}

	glLoadMatrixf(App->camera->GetViewMatrix());
	
	// Start Buffer Frame ----------------------------------
	scene_fbo.BeginFBO();

	// Object Draw Stencil Settings ------------------------
	glStencilFunc(GL_ALWAYS, 1, -1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);	

	return UPDATE_CONTINUE;
}

update_status ModuleRenderer3D::Draw()
{
	// Default Stencil Settings ----------------------------
	glStencilFunc(GL_ALWAYS, 1, 0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	// End Buffer Frame ------------------------------------
	scene_fbo.EndFBO();
	
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("[CleanUp] Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleRenderer3D::OnResize()
{
	on_resize = true;
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

void ModuleRenderer3D::SetDefaultColorMaterial()
{
	glColor4fv((float*)&render_config.default_color_mat);
}

void ModuleRenderer3D::BeginDebugDraw(float* color )
{
	if (color == nullptr)
	{
		float _color[] = { 1.f ,1.f, 1.f, 1.f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, _color);
	}
	else
	{
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, color);
	}
} 

void ModuleRenderer3D::EndDebugDraw()
{
	GLfloat emission_default[] = { 0.f, 0.f, 0.f, 1.f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission_default);
}

FBO::FBO()
{
	for (int i = 0; i < 8; ++i)
	{
		ID[i] = 0;
	}
}

FBO::~FBO()
{
	DeleteFBO();
}

void FBO::BeginFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, ID[MULTISAMPLING_FBO]);
	glClearColor(0.1, 0.1, 0.1, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FBO::EndFBO()
{

	glBindFramebuffer(GL_READ_FRAMEBUFFER, ID[MULTISAMPLING_FBO]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ID[NORMAL_FBO]);
	
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT  , GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, ID[NORMAL_TEXTURE]);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void FBO::UpdateFBO( float width, float height)
{
	this->width = width;
	this->height = height;
	bool fboUsed = true;
	// Normal =====================================================================

	// Texture ---------------------------------------------

	glBindTexture(GL_TEXTURE_2D, ID[NORMAL_TEXTURE]);
	
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
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


void FBO::GenerateFBO()
{
	glGenTextures(1,		&ID[NORMAL_TEXTURE]);
	glGenRenderbuffers(1,	&ID[NORMAL_DEPTH_RBO]);
	glGenFramebuffers(1,	&ID[NORMAL_FBO]);

	glGenRenderbuffers(1,	&ID[MULTISAMPLING_COLOR_RBO]);
	glGenRenderbuffers(1,	&ID[MULTISAMPLING_DEPTH_RBO]);
	glGenFramebuffers(1,	&ID[MULTISAMPLING_FBO]);
}

void FBO::DeleteFBO()
{
	glDeleteFramebuffers(1, &ID[MULTISAMPLING_FBO]);
	glDeleteFramebuffers(1, &ID[MULTISAMPLING_COLOR_RBO]);
	glDeleteFramebuffers(1, &ID[MULTISAMPLING_DEPTH_RBO]);

	glDeleteFramebuffers(1, &ID[NORMAL_FBO]);
	glDeleteFramebuffers(1, &ID[NORMAL_DEPTH_RBO]);
	glDeleteFramebuffers(1, &ID[NORMAL_TEXTURE]);
}

uint FBO::GetFinalTexture()
{
	return ID[NORMAL_TEXTURE];
}




