#include "Globals.h"
#include "Viewport.h"
#include "GameObject.h"
#include "C_Camera.h"

#include "ModuleRenderer3D.h"
#include <gl\glew.h>

Viewport::Viewport(GameObject* camera_go)
{
	SetCamera(camera_go);
	fbo = new FBO();
}

Viewport::~Viewport()
{
	camera = nullptr;
	delete fbo;
}

void Viewport::SetCamera(GameObject* camera_go)
{
	camera = camera_go->GetComponent< C_Camera>();
}

C_Camera* Viewport::GetCamera()
{
	return camera;
}

void Viewport::BeginViewport()
{
	if (to_update)
	{
		fbo->UpdateFBO(width, height);
		to_update = false;
	}

	// Start Buffer Frame ----------------------------------

	fbo->BeginFBO();

	// Load matrices ----------------------------------------

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(camera->GetProjectionMatrix().Transposed().ptr());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(camera->GetViewMatrix().Transposed().ptr());
}

void Viewport::EndViewport()
{
	// Default Stencil Settings ----------------------------
	glStencilFunc(GL_ALWAYS, 1, 0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	fbo->EndFBO();
}

void Viewport::SetSize(float width, float height)
{
	to_update = true;
	this->width = width;
	this->height = height;

	camera->SetAspectRatio(width, height);
}

void Viewport::SetMSAA(uint mssa)
{
	fbo->SetMSAA(mssa);
	to_update = true;
}

uint Viewport::GetMSAA()
{
	return fbo->GetMSAA();
}

uint Viewport::GetTexture()
{
	return fbo->GetFBOTexture();
}
