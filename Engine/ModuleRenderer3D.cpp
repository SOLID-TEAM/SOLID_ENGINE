#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleEditor.h"
#include "W_Scene.h"

#include <gl\glew.h>
#include "SDL\include\SDL_opengl.h"

#include <gl\GL.h>
#include <gl\GLU.h>

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
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

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
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
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
	GenSceneBuffers();

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	if (on_resize)
	{
		ImVec2 size = App->editor->w_scene->GetViewportSize();

		glViewport(0, 0, size.x, size.y);

		glMatrixMode(GL_PROJECTION);
		projection_mat = perspective(60.0f, size.x / size.y, 0.125f, 512.0f);
		glLoadMatrixf(&projection_mat);
		glMatrixMode(GL_MODELVIEW);

		UpdateSceneBuffers(size.x, size.y);

		on_resize = false;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	return UPDATE_CONTINUE;
}

update_status ModuleRenderer3D::Update(float dt)
{
	// NOT NEEDED EVERY FRAME, MOVED TO PANEL RENDER
	/*if (render_config.gl_color_material)
		glEnable(GL_COLOR_MATERIAL);
	else
		glDisable(GL_COLOR_MATERIAL);

	if (render_config.gl_depth_test)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	if (render_config.gl_cull_face)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	if (render_config.gl_lighting)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);*/

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{

	SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("[CleanUp] Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleRenderer3D::GenSceneBuffers()
{
	// Gen depth buffer ---------------------------------------
	glGenRenderbuffers(1, &depth_buffer_id);

	// Gen color texture ----------------------------------------------
	glGenTextures(1, &texture_id);

	// Gen frame buffer ----------------------------------------
	glGenFramebuffers(1, &frame_buffer_id);
}

void ModuleRenderer3D::UpdateSceneBuffers(int width, int height)
{
	// Config depth storage -----------------------------------
	glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Config  color texture ------------------------------------------
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Attach texture and render buffer to frame buffer -------
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_buffer_id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);

	// If program can generate the texture ----------------------
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG("[Error] creating screen buffer");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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