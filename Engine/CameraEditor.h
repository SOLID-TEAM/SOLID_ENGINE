#pragma once
#include "Module.h"
#include "Globals.h"
#include "GameObject.h"
#include "C_Camera.h"

#include "external/MathGeoLib/include/MathGeoLib.h"

class CameraEditor : public GameObject
{

public:
	CameraEditor();

	void Start();
	void Update();
	void CleanUp();

	float4x4 GetViewMatrix();
	float4x4 GetProjectionMatrix();

	void LookAt(float3 reference);

	void SetPosition(float3 position);

	void SetRotation(float3 rotation);

public:
	
	enum CameraState
	{
		ORBIT,
		LOOK_AROUND,
		IDLE
	} state = IDLE;

	float distance = 0;

	bool enable_mouse_input = false;
	bool mouse_right_pressed = false;
	bool mouse_wheel_pressed = false;
	bool mouse_left_pressed = false;
	bool alt_pressed = false;

private:

	C_Camera* camera = nullptr;
	bool focusing = false;

	math::float3 reference;
	math::float3 final_position;
	float final_yaw = 0.f;
	float final_pitch = 0.f;
	float current_pitch = 0.f;
	float current_yaw = 0.f;
	// Speeds -----------------------

	float lerp_trans_speed = 6.f;
	float lerp_rot_speed = 14.5f;

};