
#pragma once
#include "Color.h"

#include "external/MathGeoLib/include/Math/float3.h"

struct Light
{
	Light();

	void Init();
	void SetPos(float3 pos);
	void Active(bool active);
	void Render();

	Color ambient;
	Color diffuse;
	float3 position;

	int ref;
	bool on;
};