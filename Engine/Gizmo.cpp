//#include "gizmo.h"
//#include "sdl/include/sdl.h"
//#include "application.h"
//#include "moduleinput.h"
//
//#include <windows.h>		// header file for windows
//#include <stdio.h>			// header file for standard input/output
//#include <gl\gl.h>			// header file for the opengl32 library
//
//#include "igizmo/igizmo.h"
//
//
//gizmo::gizmo(float scale)
//{
//	gizmo_move = createmovegizmo();
//	gizmo_rotate = createrotategizmo();
//	gizmo_scale = createscalegizmo();
//
//	gizmo = gizmo_move;
//
//	gizmo->seteditmatrix(objectmatrix);
//	gizmo->setscreendimension(screenwidth, screenheight);
//
//	gizmo_move->setdisplayscale(scale);
//	gizmo_rotate->setdisplayscale(scale);
//	gizmo_scale->setdisplayscale(scale);
//}
//
//gizmo::~gizmo()
//{
//	delete gizmo_move;
//	delete gizmo_rotate;
//	delete gizmo_scale;
//
//	gizmo = nullptr;
//	gizmo_move = nullptr;
//	gizmo_rotate = nullptr;
//	gizmo_scale = nullptr;
//}
//
//void gizmo::update()
//{
//	float2 mouse_pos(app->input->getmousex(), app->input->getmousey());
//
//	if (app->input->getmousebutton(0) == key_down )
//	{
//		gizmo->onmousedown(mouse_pos.x, mouse_pos.y);
//	}
//	else if (app->input->getmousebutton(0) == key_repeat)
//	{
//		gizmo->onmousemove(mouse_pos.x, mouse_pos.y);
//	}
//	else if (app->input->getmousebutton(0) == key_up)
//	{
//		gizmo->onmouseup(mouse_pos.x, mouse_pos.y);
//	}
//}
//
//void gizmo::render()
//{
//	gizmo->draw();
//}
