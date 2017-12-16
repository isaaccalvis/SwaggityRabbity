#ifndef __gui_button_h__
#define __gui_button_h__

#include "ModuleGui.h"
#include "gui_image.h"

struct GUI_button : public GUI_object {
	GUI_button(int x, int y, SDL_Rect rect, SDL_Texture* tex, SDL_Texture* texOnMouse, SDL_Texture* texOnClick, SDL_Rect rectOnMouse, SDL_Rect rectOnClick, GUI_object* parent = nullptr);
	SDL_Texture* textureOnMouse;
	SDL_Texture* textureOnClick;
	SDL_Rect rectOnMouse;
	SDL_Rect rectOnClick;
	virtual void UpdateObject();
	void Draw();

	void CleanUp();

	virtual void MouseInFunction();
	virtual void MouseOutFunction();
	virtual void MouseClikOnFunction();
	virtual void MouseClikOffFunction();
protected:
	SDL_Texture* textureToDraw = nullptr;
	SDL_Rect *rectToDraw = nullptr;
	GUI_image* image = nullptr;
};

#endif