#include "C_Mesh.h"

ComponentMesh::ComponentMesh(GameObject* parent) : Component(parent) {}

ComponentMesh::~ComponentMesh() {}

bool ComponentMesh::Update(float dt)
{
	return true;
}