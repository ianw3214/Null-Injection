#pragma once

#include "QcEngine.h"

#include "../entities/background.h"

#define TIMER_X 20
#define TIMER_Y 120

#define YOUR_TIME_X 30
#define YOUR_TIME_Y 50

#define FINAL_FADE_OUT_TIME 2000

#define TEXT_1_TIME 500
#define TEXT_2_TIME 1000
#define TEXT_3_TIME 1500
#define TEXT_4_TIME 2000

#define MOVE_INTERVAL 500

#define ITEM_X_POS 20

#define FINAL_SHAKE_INTERVAL 50
#define FINAL_SHAKE_INTENSITY 30

#define PERFECT_GAME_X 20
#define PERFECT_GAME_Y 476

class FinalState : public State{

public:
	FinalState(int finalTime, int health);
	~FinalState();

	void init();
	void cleanUp();

	void update(Uint32 delta);
	void render(SDL_Renderer * renderer);
private:

	Background * background;
	Texture * yourTime;

	int currentItem;
	std::vector<Texture*> menuItems;

	void updateMenuItems();
	void select();

	int keyUpTimer;
	int keyDownTimer;

	Texture * black;
	int fadeOutTimer;
	bool fadeOut;

	void startGame();

	// variables for score
	int time;
	Texture * timeTexture;

	// load in variables
	bool loadin;

	int startTime;
	bool titlePlayed;
	bool timePlayed;
	bool itemPlayed;

	// camera shake variables
	int xOffset, yOffset;
	int shakeTimer;

	// perfect game things
	Texture * perfectGameText;
	bool perfectGame;
};