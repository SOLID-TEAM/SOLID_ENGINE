#include "C_Mesh.h"
#include "Application.h"
#include "GL/glew.h"
#include "ImGui/imgui.h"

ComponentMesh::ComponentMesh(GameObject* parent) : Component(parent)
{
	name.assign("Mesh");
	mesh = new ModelData();
}

ComponentMesh::~ComponentMesh() {}

bool ComponentMesh::Update(float dt)
{
	return true;
}

bool ComponentMesh::PostUpdate(float dt)
{
	return true;
}

bool ComponentMesh::Draw()
{
	if (mesh != nullptr)
	{
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		/*mesh->Render();*/

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glLineWidth(1.0f);
		//glColor4fv((float*)&wire_color);

		mesh->RenderWireframe();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	return true;
}

bool ComponentMesh::CleanUp()
{
	delete mesh;

	return true;
}