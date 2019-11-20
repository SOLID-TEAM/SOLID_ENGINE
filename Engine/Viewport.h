#ifndef _VIEWPORT_H__
#define _VIEWPORT_H__

class FBO
{
public:

	enum BufferType
	{
		DEPTH_TEAXTURE,
		NORMAL_FBO,
		NORMAL_TEXTURE,
		NORMAL_DEPTH_RBO,
		MULTISAMPLING_FBO,
		MULTISAMPLING_COLOR_RBO,
		MULTISAMPLING_DEPTH_RBO,
	};

	FBO();

	~FBO();

	void BeginFBO();

	void EndFBO();

	void GenerateFBO();

	void UpdateFBO(float width, float height);

	void DeleteFBO();

	uint GetFBOTexture();

	// Only multiple of 2 values // 0 : MSAA disabled // 2-16 : MSAA enabled
	void SetMSAA(uint MSAA)
	{
		if (MSAA % 2 != 0 || MSAA > 0 || MSAA <= 16)
		{
			msaa = MSAA;
		}
	}

	uint GetMSAA()
	{
		return msaa;
	}

public:

	float4 clear_color = { 0.1, 0.1, 0.1, 1.f };

private:

	bool z_buffer_mode = true;
	uint ID[8];
	uint msaa = 4;
	float2 position;

	float width = 0, height = 0;
};


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

	void SetPos(float2 position);

	void SetSize(float width, float height);

	void SetMSAA(uint mssa);

	uint GetMSAA();

	uint GetTexture();

	bool ScreenPointToViewport(float2& screen_point);

public:

	bool		active = false;

private:

	FBO*		fbo = nullptr;
	bool        to_update = true;
	C_Camera*	camera = nullptr;

	float2		position;
	float		width = 640.f, height = 360.f;
};

#endif // !_VIEWPORT_H__
