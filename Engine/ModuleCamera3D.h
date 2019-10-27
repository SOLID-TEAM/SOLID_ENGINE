#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"

#include "external/MathGeoLib\include\Math\float3.h"
#include "external/MathGeoLib\include\Math\float4x4.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Start(Config& config);
	update_status Update(float dt);
	bool CleanUp();

	void Load(Config& config);
	bool Save(Config& config);
	void LookAt(const math::float3&  Spot);
	void Move(const math::float3& Movement);
	float* GetViewMatrix();

private:

	void CalculateViewMatrix();

public:
	
	enum CameraState
	{
		ORBIT,
		LOOK_AROUND,
		IDLE
	} state = IDLE;

	float distance = 0;

	bool mouse_right_pressed = false;
	bool mouse_left_pressed = false;
	bool alt_pressed = false;

	bool enable_keys_input = false;
	bool enable_mouse_input = false;
	math::float3 X, Y, Z, position, reference;

private:
	math::float4x4 view_matrix;
};