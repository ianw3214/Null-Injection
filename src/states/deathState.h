#pragma once

#include <vector>

#include "QcEngine.h"

#define KEY_TIMER 200

class DeathMenu : public State {

public:
	DeathMenu();
	~DeathMenu();

	void init();
	void cleanUp();

	void update(Uint32 delta);
	void render(SDL_Renderer * renderer);
private:

	int currentItem;
	std::vector<Texture*> menuItems;

	void updateMenuItems();
	void select();

	int keyUpTimer;
	int keyDownTimer;

};