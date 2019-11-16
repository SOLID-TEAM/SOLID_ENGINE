#ifndef _VIEWPORT_H__
#define _VIEWPORT_H__


class GameObject;
class C_Camera;
class FBO;

class Viewport
{
public:

	Viewport(GameObject* camera);

	~Viewport();

	void SetCamera(GameObject* camera_go);

	C_Camera* GetCamera();

	void BeginViewport();

	void EndViewport();

	void SetSize(float width, float height);

	void SetMSAA(uint mssa);

	uint GetMSAA();

	uint GetTexture();

public:

	bool		active = false;

private:

	FBO*		fbo = nullptr;
	bool        to_update = true;
	C_Camera*	camera = nullptr;
	float		width = 640.f, height = 360.f;
};

#endif // !_VIEWPORT_H__
