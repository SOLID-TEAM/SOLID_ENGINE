// TODO: delete when fully tested new method
//#include "GL/glew.h"
//#include "Globals.h"
//#include <gl/GL.h>
//#include <gl/GLU.h>
//
//
//#include "Primitive.h"
//#include "external/par_shapes.h"
//
//Primitive::Primitive() : transform(IdentityMatrix)
//{}
//
//void Primitive::SetPos(float x, float y, float z)
//{
//	transform.translate(x, y, z);
//}
//
//void Primitive::SetRotation(float angle, const vec3 &u)
//{
//	transform.rotate(angle, u);
//}
//
//void Primitive::Scale(float x, float y, float z)
//{
//	par_shapes_scale(data, x, y, z);
//	GenBuffer();
//}
//
//void Primitive::GenBuffer()
//{
//	//generate buffer for vertices
//	glGenBuffers(1, &vector_id);
//	glBindBuffer(GL_ARRAY_BUFFER, vector_id);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * data->npoints, &data->points[0], GL_STATIC_DRAW);
//
//	// generate buffer for indices
//	glGenBuffers(1, &elements_id);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements_id);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * data->ntriangles * 3, &data->triangles[0], GL_STATIC_DRAW);
//}
//
//void Primitive::Render()
//{
//	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//
//	/*glEnable(GL_POLYGON_OFFSET_FILL);
//	glPolygonOffset(-1.0f, 0.0f);*/
//
//	//glColor3f(1.0f, 0.0f, 1.0f);
//
//	glEnableClientState(GL_VERTEX_ARRAY);
//
//	glBindBuffer(GL_ARRAY_BUFFER, vector_id);
//	glVertexPointer(3, GL_FLOAT, 0, (void*)0);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements_id);
//	glDrawElements(GL_TRIANGLES, data->ntriangles * 3, GL_UNSIGNED_INT, (void*)0);
//
//	glDisableClientState(GL_VERTEX_ARRAY);
//}
//
//// Cube ============================================
//
//S_Cube::S_Cube(float x, float y, float z) : Primitive()
//{
//	data = par_shapes_create_cube();
//	//par_shapes_translate(data, x, y, z);
//	GenBuffer();
//}
//
//// Sphere ============================================
//
//S_Sphere::S_Sphere(float x, float y, float z ,float radius) : Primitive(), radius(radius)
//{
//	data = par_shapes_create_parametric_sphere(20, 20);
//	par_shapes_translate(data, x, y, z);
//	GenBuffer();
//}
//
//// Plane ============================================
//
//S_Plane::S_Plane(float x, float y, float z)
//{
//	data = par_shapes_create_plane(4, 4);
//	par_shapes_translate(data, x, y, z);
//	GenBuffer();
//}
