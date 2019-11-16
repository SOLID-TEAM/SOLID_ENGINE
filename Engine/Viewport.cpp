#include "Globals.h"
#include "Application.h"
#include "Viewport.h"
#include "GameObject.h"
#include "C_Camera.h"

#include "ModuleRenderer3D.h"
#include "ModuleScene.h"
#include <gl\glew.h>

Viewport::Viewport(GameObject* camera_go)
{
	App->scene->viewports.push_back(this);
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
	to_update = true;
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

	// Enbales ----------------------------------------------

	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

	glShadeModel(GL_SMOOTH);                    // shading mathod: GL_SMOOTH or GL_FLAT
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);      // 4-byte pixel alignment

	// Default Stencil Settings ----------------------------

	glStencilFunc(GL_ALWAYS, 1, 0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	// Default Depth Settings ----------------------------
	glDepthMask(true);
	glDepthRange(0.f, 1.f);

}

void Viewport::EndViewport()
{
	// Disables --------------------------------------------
	glDisable(GL_LIGHTING);
	glDisable(GL_POLYGON_SMOOTH);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);


	fbo->EndFBO();
}

void Viewport::SetSize(float width, float height)
{
	this->width = width;
	this->height = height;

	camera->SetAspectRatio(width, height);

	to_update = true;
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
