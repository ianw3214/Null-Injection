#pragma once

#include <vector>

#include "QcEngine.h"

#define KEY_TIMER 200
#define FADE_OUT_TIME 2000

class Menu : public State {

public:
	Menu();
	~Menu();

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

	Texture * black;
	int fadeOutStart;
	bool fadeOut;

	void startGame();

	std::vector<Texture*> controlText;
	std::vector<Texture*> credits;
	void renderControlText();
	void renderCredits();
	bool renderControl;
	bool renderCredit;

	bool fullScreen;
};