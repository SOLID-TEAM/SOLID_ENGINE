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
	void Update(float dt);
	void CleanUp();

	float4x4 GetViewMatrix();
	float4x4 GetProjectionMatrix();

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
	bool focusing = false;
	math::float3 final_position;
	math::Quat final_rotation;
	math::float3 reference;
	C_Camera* camera = nullptr;
};