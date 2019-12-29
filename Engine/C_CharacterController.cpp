#include "C_CharacterController.h"
#include "C_RigidBody.h"
#include "C_Collider.h"
#include "Application.h"
#include "GameObject.h"
#include "ModuleInput.h"
#include "ModuleTime.h"
#include "W_Game.h"

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
	float force = 20.f;

	if (current_collider == nullptr)
		linked_go->AddComponent<C_CapsuleCollider>();
	if (current_rb == nullptr)
		linked_go->AddComponent<C_RigidBody>();

	if (App->time->GetGameState() != GameState::STOP)
	{
		App->editor->w_game->GetViewportSize();
		//linked_go->transform->SetRotation();

		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
			current_rb->AddForce( float3::unitY * 10.F);

		if (App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN)
			current_rb->AddTorque(float3::unitY * 0.5f);

		float3 turn(0.f, 0.f, 0.f);

		if (App->input->GetKey(SDL_SCANCODE_Y) == KEY_REPEAT)
		{
			current_rb->SetAngularVelocity(float3::unitY * 2);
		}
		else if (App->input->GetKey(SDL_SCANCODE_I) == KEY_REPEAT)
		{
			current_rb->SetAngularVelocity(- float3::unitY * 2);
		}
		else
		{
			current_rb->SetAngularVelocity(float3::zero);
		}

		float3 movement(0.f, 0.f, 0.f);

		if (App->input->GetKey(SDL_SCANCODE_U) == KEY_REPEAT)
			movement -= float3::unitZ;

		if (App->input->GetKey(SDL_SCANCODE_J) == KEY_REPEAT)
			movement += float3::unitZ;

		if (App->input->GetKey(SDL_SCANCODE_H) == KEY_REPEAT)
			movement -= float3::unitX;

		if (App->input->GetKey(SDL_SCANCODE_K) == KEY_REPEAT)
			movement += float3::unitX;

		if(!movement.Equals(float3::zero))
		{
			current_rb->AddForce(movement.Normalized() * force, ForceMode::FORCE);
		}

		//LOG("Velocity" , )
	}


	return false;
}



bool C_CharacterController::DrawPanelInfo()
{
	return false;
}

