#ifndef _MODULE_H__
#define _MODULE_H__

#include <string>

class Config;
struct Event;

class Module
{
private :
	bool enabled;
protected:
	std::string name;

public:

	Module(bool start_enabled = true) : enabled(start_enabled)
	{}

	virtual ~Module()
	{}

	virtual bool Init(Config& config) 
	{
		return true; 
	}

	virtual bool Start(Config& config)
	{
		return true;
	}

	virtual update_status PreUpdate()
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update()
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate()
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Draw() // used only for opengl draw on viewport
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	/*virtual void OnCollision(PhysBody3D* body1, PhysBody3D* body2)
	{}*/

	// ---------------------------------------------------
	bool IsActive() const
	{
		return enabled;
	}

	const char* GetName()
	{
		return name.data();
	}

	virtual void ReceiveEvent(const Event& event) {}

	virtual bool Save(Config& config)
	{
		return true;
	}

	virtual void Load(Config& config)
	{
		//return true;
	}
};

#endif // !_MODULE_H__