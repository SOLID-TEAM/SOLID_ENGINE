//#include "Gizmo.h"
//#include "SDL/include/SDL.h"
//#include "Application.h"
//#include "ModuleInput.h"
//
//
//#include "iGizmo/IGizmo.h"
//
//Gizmo::Gizmo(float scale)
//{
//	gizmo_move = CreateMoveGizmo();
//	gizmo_rotate = CreateRotateGizmo();
//	gizmo_scale = CreateScaleGizmo();
//
//	gizmo = gizmo_move;
//
//	//gizmo->SetEditMatrix(objectMatrix);
//	//gizmo->SetScreenDimension(screenWidth, screenHeight);
//
//	gizmo_move->SetDisplayScale(scale);
//	gizmo_rotate->SetDisplayScale(scale);
//	gizmo_scale->SetDisplayScale(scale);
//}
//
//Gizmo::~Gizmo()
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
//void Gizmo::Update()
//{
//	float2 mouse_pos(App->input->GetMouseX(), App->input->GetMouseY());
//
//	if (App->input->GetMouseButton(0) == KEY_DOWN )
//	{
//		gizmo->OnMouseDown(mouse_pos.x, mouse_pos.y);
//	}
//	else if (App->input->GetMouseButton(0) == KEY_REPEAT)
//	{
//		gizmo->OnMouseMove(mouse_pos.x, mouse_pos.y);
//	}
//	else if (App->input->GetMouseButton(0) == KEY_UP)
//	{
//		gizmo->OnMouseUp(mouse_pos.x, mouse_pos.y);
//	}
//}
//
//void Gizmo::Render()
//{
//
//}
