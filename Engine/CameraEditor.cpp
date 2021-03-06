#include "Globals.h"
#include "Application.h"
#include "CameraEditor.h"
#include "ModuleScene.h"
#include "ModuleTime.h"
#include "GameObject.h"
#include "C_Transform.h"

CameraEditor::CameraEditor() : GameObject("Camera Editor", false, nullptr)
{
	camera = AddComponent<C_Camera>();
	camera->SetClippingFarPlane(800.f);
	camera->SetFov(60.f);

	reference = { 0.0f, 0.0f, 0.0f };
	final_position = { 0.0f, 0.0f, 0.0f };
	final_pitch = 0.f;
	final_yaw = 0.f;
}

void CameraEditor::Start()
{
	
}

void CameraEditor::CleanUp()
{
}

float4x4 CameraEditor::GetViewMatrix()
{
	return camera->GetViewMatrix();
}

float4x4 CameraEditor::GetProjectionMatrix()
{
	return camera->GetProjectionMatrix();
}

void CameraEditor::SetPosition(float3 position)
{
	transform->SetPosition(position);
	final_position = transform->position;
}

void CameraEditor::SetRotation(float3 rotation)
{
	transform->SetRotation(rotation);
	current_pitch = final_pitch = transform->rotation.x;
	current_yaw = final_yaw = transform->rotation.y;
}

void CameraEditor::Update()
{
	float dt = App->time->UnscaledDeltaTime();

	mouse_right_pressed = (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT || App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_DOWN) ;
	mouse_left_pressed = (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT || App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN);
	mouse_wheel_pressed = (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT || App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_DOWN);
	alt_pressed = (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RALT) == KEY_REPEAT  || App->input->GetKey(SDL_SCANCODE_LALT) == KEY_DOWN|| App->input->GetKey(SDL_SCANCODE_RALT) == KEY_DOWN);


	const float mouse_z = App->input->GetMouseZ();
	const float mouse_motion_x = -App->input->GetMouseXMotion();
	const float mouse_motion_y = App->input->GetMouseYMotion();

	float3 current_position = transform->position;

	state = IDLE;

	if (enable_mouse_input)
	{
		if (mouse_right_pressed)
		{
 			focusing = false;

			// Look Around -------------------------------------

			state = LOOK_AROUND;

			if (mouse_motion_x != 0.f || mouse_motion_y != 0.f)
			{
				float rotation_speed = 7.f * dt;
				float yaw = mouse_motion_x * rotation_speed;
				float pitch = mouse_motion_y * rotation_speed;

				final_yaw += yaw;
				final_pitch += pitch;
			}

			// WASD movement ----------------------------------

			math::float3 offset(0, 0, 0);

			float speed = 7.f * dt;

			if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
			{
				speed *= 2;
			}

			if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) offset -= transform->forward * speed;
			if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) offset += transform->forward * speed;

			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) offset -= transform->right * speed;
			if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) offset += transform->right * speed;

			if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) offset -= transform->up * speed;
			if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) offset += transform->up * speed;

			final_position -= offset;

			// Lerp ---------------------------------------------------------------

			current_yaw = math::Lerp(current_yaw, final_yaw, lerp_rot_speed * dt);
			current_pitch = math::Lerp(current_pitch, final_pitch, lerp_rot_speed * dt);

			transform->SetRotation(Quat::RotateAxisAngle(float3::unitY, current_yaw * DEGTORAD) * Quat::RotateAxisAngle(float3::unitX, current_pitch * DEGTORAD));
			transform->SetPosition(float3::Lerp(current_position, final_position, lerp_trans_speed * dt));
		}
		else
		{
			// Orbit -------------------------------------------------------------------

			if (alt_pressed)
			{
				if (mouse_left_pressed)
				{
					focusing = false;

					if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
					{
						distance = (transform->position - reference).Length();
					}

					state = ORBIT;

					if (mouse_motion_x != 0.f || mouse_motion_y != 0.f)
					{
						float rotation_speed = 7.f * dt;
						float yaw = mouse_motion_x * rotation_speed;
						float pitch = mouse_motion_y * rotation_speed;

						current_yaw = final_yaw += yaw;
						current_pitch = final_pitch += pitch;
						transform->SetRotation(Quat::RotateAxisAngle(float3::unitY, current_yaw * DEGTORAD) * Quat::RotateAxisAngle(float3::unitX, current_pitch * DEGTORAD));

						current_position = reference + -transform->forward * distance;
						transform->SetPosition(current_position);
					}
				}
			}

			// Zoom ---------------------------------------------------------------------


			if (mouse_z != 0 && state != LOOK_AROUND)
			{
				float zoom_speed = 30.f * dt;
				focusing = false;

				if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT)
				{
					zoom_speed *= 4.f;
				}

				math::float3 offset = zoom_speed * transform->forward * mouse_z;

				current_position += offset;
				transform->SetPosition(current_position);

				distance = (current_position - reference).Length();
			}

			// Pan ---------------------------------------------------------------------

			if (mouse_wheel_pressed && state == IDLE)
			{
				float palm_speed = 1.f * dt;

				if (mouse_motion_x != 0 || mouse_motion_y != 0)
				{
					math::float3 offset(0, 0, 0);
					focusing = false;

					offset -= transform->right * palm_speed * mouse_motion_x;
					offset += transform->up * palm_speed * mouse_motion_y;

					current_position += offset;
					transform->SetPosition(current_position);
				}
			}

			// Focus -----------------------------------------------------------------

			if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN && state == IDLE)
			{
				GameObject* selected = App->scene->selected_go;

				if (selected != nullptr && App->scene->root_go != selected)
				{
					AABB general_aabb = selected->GetHierarchyAABB();
					Sphere sphere = general_aabb.MinimalEnclosingSphere();

					if (sphere.Diameter() != 0)
					{
						distance = sphere.Diameter() * 1.2f;
						float3 d_vector = -transform->forward * distance;
						reference = general_aabb.CenterPoint();
						final_position = reference + d_vector;
					}
					else
					{
						distance = 3.f;
						float3 d_vector = - transform->forward * distance;
						reference = selected->transform->position;
						final_position = reference + d_vector;
					}

					focusing = true;
				}
			}

			if (focusing == true)
			{
				transform->SetPosition(float3::Lerp(transform->position, final_position, lerp_trans_speed * dt));

				if (transform->position.Distance(final_position) <= 0.001f)
				{
					focusing = false;
				}
			}
			else
			{
				final_position = current_position;
			}
		}
	}

		

		
	
}


