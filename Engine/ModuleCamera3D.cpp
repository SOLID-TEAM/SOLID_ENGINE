#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "external/MathGeoLib\include\Math\MathAll.h"
ModuleCamera3D::ModuleCamera3D(bool start_enabled) : Module(start_enabled)
{
	name.assign("Camera3D");

	CalculateViewMatrix();

	X = {1.0f, 0.0f, 0.0f};
	Y = {0.0f, 1.0f, 0.0f};
	Z = {0.0f, 0.0f, 1.0f};

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
	// Keys motion ----------------------------------

	if (enable_keys_input)
	{
		math::float3 offset(0, 0, 0);

		float speed = 5.f * dt;

		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		{
			speed *= 2;
		}

		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT) offset.y += speed;
		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT) offset.y -= speed;

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) offset -= Z * speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) offset += Z * speed;


		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) offset -= X * speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) offset += X * speed;

		position += offset;
		reference += offset;
	}

	// Mouse motion ------------------------------

	mouse_right_presed = (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT || App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_DOWN);

	if (enable_mouse_input)
	{
		// Look Around ----------------------------------------------------------------

		if (mouse_right_presed)
		{
			float dx = -App->input->GetMouseXMotion(); 
			float dy = -App->input->GetMouseYMotion(); 

			//position -= reference;

			if (dx != 0 || dy != 0)
			{
				float rotation_speed = 10.f * dt;
				float yaw = dx * rotation_speed;
				float pitch = dy * rotation_speed;

				math::Quat rot_x = math::Quat::RotateAxisAngle(math::float3::unitY , -yaw * DEGTORAD);
				math::Quat rot_y = math::Quat::RotateAxisAngle(math::Cross(Z, Y), -pitch * DEGTORAD);
				math::Quat final_rot = rot_x * rot_y;

				Z = final_rot * Z;
				Y = final_rot * Y;
				X = math::Cross(Z, Y);
			}

			//position = reference ;
		}

		// Zoom ---------------------------------------------------------------------

		float mouse_z = - App->input->GetMouseZ();

		if (mouse_z != 0)
		{
			float zoom_speed = 50.f * dt;

			if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT)
			{
				zoom_speed *= 4.f; 
			}

			math::float3 offset = zoom_speed * Z * mouse_z ;
			position += offset;
			reference += offset;
		}	

	}

	// Recalculate matrix -------------

	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const math::float3 & pos, const math::float3 & ref, bool rotate_around)
{
	this->position = pos;
	this->reference = ref;

	Z = (pos - ref).Normalized();
	math::float3 eye_Y(0.0f, 1.0f, 0.0f);
	X = eye_Y.Cross(Z).Normalized();
	Y = Z.Cross(X);

	if(!rotate_around)
	{
		this->reference = this->position;
		this->position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt(const math::float3 &spot)
{
	reference = spot;

	Z = (position - reference).Normalized();
	math::float3 eye_Y(0.0f, 1.0f, 0.0f);

	X = eye_Y.Cross(Z).Normalized();
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
	return &ViewMatrix;
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, - X.Dot(position), -Y.Dot(position), -Z.Dot(position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
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