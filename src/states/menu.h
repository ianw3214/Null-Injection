#pragma once

#include <vector>

#include "QcEngine.h"
#include "../entities/background.h"

#define KEY_TIMER 200
#define MENU_FADE_OUT_TIME 2000

#define MENU_TITLE_X 15
#define MENU_TITLE_Y 15

#define HINT_INTERVAL 500
#define HINT_X 20
#define HINT_Y 484

class Menu : public State {

public:
	Menu();
	~Menu();

	void init();
	void cleanUp();

	void update(Uint32 delta);
	void render(SDL_Renderer * renderer);
private:

	Texture * title;
	Background * background;
	int currentItem;
	std::vector<Texture*> menuItems;

	void updateMenuItems();
	void select();

	int keyUpTimer;
	int keyDownTimer;
	int selectTimer;

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

	Texture * hintText;
};