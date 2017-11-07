#pragma once

#include <vector>

#include "QcEngine.h"

#include "../entities/background.h"

#define KEY_TIMER 200
#define DEATH_FADE_OUT_TIME 2000

#define GAME_OVER_X 30
#define GAME_OVER_Y 50

class DeathMenu : public State {

public:
	DeathMenu();
	~DeathMenu();

	void init();
	void cleanUp();

	void update(Uint32 delta);
	void render(SDL_Renderer * renderer);
private:

	Background * background;
	int currentItem;
	std::vector<Texture*> menuItems;

	void updateMenuItems();
	void select();

	int keyUpTimer;
	int keyDownTimer;

	Texture * black;
	int fadeOutStart;
	bool fadeOut;

	Texture * gameOverText;

};