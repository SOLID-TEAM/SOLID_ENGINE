#include "C_CharacterController.h"
#include "C_RigidBody.h"
#include "C_Collider.h"
#include "Application.h"
#include "GameObject.h"
#include "ModuleInput.h"
#include "ModuleTime.h"

C_CharacterController::C_CharacterController(GameObject* go) : Component(go, ComponentType::CHARACTER_CONTROLLER)
{
	name.assign("Charactet Controller");
}

bool C_CharacterController::CleanUp()
{
	return false;
}

bool C_CharacterController::Save(Config& config)
{
	return false;
}

bool C_CharacterController::Load(Config& config)
{
	is_loaded = true;
	return false;
}

bool C_CharacterController::Update()
{
	C_Collider* current_collider = linked_go->GetComponent<C_Collider>();
	C_RigidBody* current_rb = linked_go->GetComponent<C_RigidBody>();
	
	if (current_collider == nullptr)
		linked_go->AddComponent<C_CapsuleCollider>();
	if (current_rb == nullptr)
		linked_go->AddComponent<C_RigidBody>();

	if (App->time->GetGameState() != GameState::STOP)
	{
		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
			current_rb->AddForce( float3::unitY * 10.F, Space::Global);

		if (App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN)
			current_rb->AddTorque(float3::unitY * 0.5F, Space::Global);
	}

	return false;
}



bool C_CharacterController::DrawPanelInfo()
{
	return false;
}

