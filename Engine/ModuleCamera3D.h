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

	void Look(const math::float3 & pos, const math::float3 & Reference, bool RotateAroundReference = false);
	void LookAt(const math::float3&  Spot);
	void Move(const math::float3& Movement);
	float* GetViewMatrix();

private:

	void CalculateViewMatrix();

public:
	
	bool mouse_right_presed = false;
	bool enable_keys_input = false;
	bool enable_mouse_input = false;
	math::float3 X, Y, Z, position, reference;

private:
	mat4x4 ViewMatrix, ViewMatrixInverse;
};