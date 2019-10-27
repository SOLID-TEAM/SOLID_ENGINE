#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"

#include "ModuleScene.h"
#include "GameObject.h"

#include "C_Transform.h"
#include "C_Mesh.h"
#include "D_Mesh.h"

#include "external/MathGeoLib\include\Math\MathAll.h"

ModuleCamera3D::ModuleCamera3D(bool start_enabled) : Module(start_enabled)
{
	name.assign("Camera3D");

	CalculateViewMatrix();

	X = float3::unitX;
	Y = float3::unitY;
	Z = float3::unitZ;

	position = { 0.0f, 0.0f, 5.0f };
	reference = { 0.0f, 0.0f, 0.0f };
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start(Config& config)
{
	LOG("[Start] Setting up the camera");
	bool ret = true;

	Load(config);

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("[CleanUp] Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	mouse_right_pressed = (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT || App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_DOWN);
	mouse_left_pressed = (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT || App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN);
	alt_pressed = (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RALT) == KEY_REPEAT  || App->input->GetKey(SDL_SCANCODE_LALT) == KEY_DOWN|| App->input->GetKey(SDL_SCANCODE_RALT) == KEY_DOWN);

	state = IDLE;

	if (enable_mouse_input)
	{
		if (mouse_right_pressed)
		{
			// Look Around -------------------------------------

			state = LOOK_AROUND;

			float dx = -App->input->GetMouseXMotion();
			float dy = -App->input->GetMouseYMotion();

			if (dx != 0 || dy != 0)
			{
				float rotation_speed = 10.f * dt;
				float yaw = dx * rotation_speed;
				float pitch = dy * rotation_speed;

				math::Quat rot_y = math::Quat::RotateAxisAngle(math::float3::unitY, yaw * DEGTORAD);
				math::Quat rot_x = math::Quat::RotateAxisAngle(math::Cross(Y, Z), pitch * DEGTORAD);
				math::Quat final_rot = rot_y * rot_x;

				Z = final_rot * Z;
				Y = final_rot * Y;
				X = Cross(Y, Z);
			}

			// Keys motion ----------------------------------

			if (enable_keys_input)
			{
				math::float3 offset(0, 0, 0);

				float speed = 5.f * dt;

				if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
				{
					speed *= 2;
				}

				if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) offset -= Z * speed;
				if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) offset += Z * speed;

				if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) offset -= X * speed;
				if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) offset += X * speed;

				if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) offset -= Y * speed;
				if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) offset += Y * speed;

				position += offset;
			}
		}

		// Orbit -------------------------------------------------------------------

		else if (alt_pressed)
		{
			if (mouse_left_pressed)
			{
				if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
				{
					distance = (position - reference).Length();
				}

				state = ORBIT;

				float dx = -App->input->GetMouseXMotion();
				float dy = -App->input->GetMouseYMotion();

				if (dx != 0 || dy != 0)
				{
					float rotation_speed = 10.f * dt;
					float yaw = dx * rotation_speed;
					float pitch = dy * rotation_speed;

					math::Quat rot_y = math::Quat::RotateAxisAngle(math::float3::unitY, yaw * DEGTORAD);
					math::Quat rot_x = math::Quat::RotateAxisAngle(math::Cross(Y, Z), pitch * DEGTORAD);
					math::Quat final_rot = rot_y * rot_x;

					Z = final_rot * Z;
					Y = final_rot * Y;
					X = Cross(Y, Z);

					position = reference + Z.Normalized() * distance;
				}
			}
		}
		
		// Zoom ---------------------------------------------------------------------

		float mouse_z = -App->input->GetMouseZ();

		if (mouse_z != 0 && state != LOOK_AROUND)
		{
			float zoom_speed = 50.f * dt;

			if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT)
			{
				zoom_speed *= 4.f;
			}

			math::float3 offset = zoom_speed * Z * mouse_z;
			position += offset;

			distance = (position - reference).Length();

		}

		// Focus -----------------------------------------------------------------

		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN && state == IDLE)
		{
			GameObject* selected = App->editor->selected_go;

			if (selected != nullptr && App->scene->root_go != selected)
			{
				AABB general_aabb;
				selected->GetBoundingBox(general_aabb);
				Sphere sphere = general_aabb.MinimalEnclosingSphere();

				if (sphere.Diameter() != 0)
				{
					distance = sphere.Diameter() * 1.2f;
					float3 d_vector = Z * distance;
					reference = general_aabb.CenterPoint();
					position = reference + d_vector;
				}
				else
				{
					distance = 3.f;
					float3 d_vector = Z * distance;
					reference = selected->transform->position;
					position = reference + d_vector;
				}

				

				//C_Mesh* c_mesh = (C_Mesh * )selected->GetComponentsByType(ComponentType::MESH);

				//if (c_mesh != nullptr)
				//{
				//	distance = c_mesh->data->aabb.Diagonal().Length() * 1.2F;
				//	float3 d_vector = Z * distance;
				//	reference = c_mesh->data->aabb.CenterPoint();
				//	position = reference + d_vector;
				//}
			}
		}


			

	}
	

	// Recalculate matrix -------------

	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt(const math::float3 &spot)
{
	reference = spot;

	Z = (position - reference).Normalized();
	X = Cross(float3::unitY, Z).Normalized();
	Y = Z.Cross(X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const math::float3 &Movement)
{
	position += Movement;
	reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return view_matrix.ptr();
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	view_matrix = float4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, - X.Dot(position), -Y.Dot(position), -Z.Dot(position), 1.0f);
}

bool ModuleCamera3D::Save(Config& config)
{
	bool ret = true;

	/*float blabla[10] = { 0,2,0,0,0,0,4,5,5,10 };

	ret = config.AddBool("about", about);
	ret = config.AddBool("showcase", showcase);
	ret = config.AddFloatArray("sdfgd", blabla, 10);*/

	return ret;
}

void ModuleCamera3D::Load(Config& config)
{
	/*about = config.GetBool("about", about);
	showcase = config.GetBool("showcase", showcase);*/
}