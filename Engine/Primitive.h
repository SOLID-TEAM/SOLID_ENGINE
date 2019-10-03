#ifndef __PRIMITIVE_H__
#define __PRIMITIVE_H__

#include "glmath.h"
#include "Color.h"
#include "par_shapes.h"

class Primitive
{
public:

	Primitive();

	void GenBuffer();
	void SetPos(float x, float y, float z);
	void SetRotation(float angle, const vec3 &u);
	void Scale(float x, float y, float z);

	virtual void Render();

protected:

	uint vector_id = 0u;
	uint elements_id = 0u;

	par_shapes_mesh* mesh = nullptr;
	mat4x4 transform;
};

// ============================================
class S_Cube : public Primitive
{
public :

	S_Cube(float x, float y, float z);

public:
	vec3 size;
};

// ============================================
class S_Sphere : public Primitive
{
public:

	S_Sphere(float x, float y, float z, float radius);

public:

	float radius;
};

class S_Plane : public Primitive
{
public:

	S_Plane(float x, float y, float z);

public:

	
};

#endif