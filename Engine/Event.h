#ifndef __EVENT_H__
#define __EVENT_H__

struct Event
{
	enum EventType
	{
		file_dropped,
		play,
		pause,
		resume,
		stop,
		gameobject_deleted,
		window_resized,
		invalid
	} type;

	union
	{
		struct
		{
			const char* ptr;
		} string;

		struct
		{
			int x, y;
		} point2d;
	};

	Event(EventType type) : type(type)
	{}
};


#endif // __EVENT_H__