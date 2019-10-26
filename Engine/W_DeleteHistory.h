#ifndef __PANEL_DELETEHISTORY_H__
#define __PANEL_DELETEHISTORY_H__

#include "Globals.h"
#include "Window.h"

#include <vector>

class W_DeleteHistory : public Window
{
public:

	W_DeleteHistory(std::string name, bool active);

	virtual ~W_DeleteHistory();

	void Draw();


private:

};

#endif // __PANEL_DELETEHISTORY_H__