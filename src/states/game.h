#pragma once

#include <vector>
#include <algorithm>
#include <memory>
#include <fstream>
#include <string>

#include "QcEngine.h"
#include "../attackMessage.h"
#include "../constants.h"
#include "../entities/player.h"
#include "../entities/enemy.h"
#include "../entities/map.h"
#include "../audio/audio.h"

#define CAM_SPEED 300
#define CAM_MARGIN 120
#define HEALTH_MARGIN_Y 20
#define HEALTH_MARGIN_X 10
#define INTRO_FADE_BAR 400
#define INTRO_FADE_DELAY 40
#define PLAYER_ATTACK_SHAKE_TIME 80
#define PLAYER_ATTACK_SHAKE_INTENSITY 10

#define SWITCH_DEATH_MENU 500
#define SLOW_MO_FRAMES 3
#define NUM_SLOW_MO_FRAMES 20
#define RED_OVERLAY_PERSIST_AFTER 3000
#define DEATH_SHAKE_TIME 800
#define DEATH_SHAKE_INTENSITY 50
#define TITLE_X 50
#define TITLE_Y 100
#define SHOW_TITLE_AT 2500
#define TITLE_FULL_AT 3500
#define DEATH_TEXT_X 30
#define DEATH_TEXT_Y 300
#define SHOW_DEATH_TEXT_AT 4000
#define DEATH_TEXT_BLINK_TIME 500



class Game : public State {

public:
	Game();
	Game(std::string path);
	~Game();

	void init();
	void cleanUp();

	void update(Uint32 delta);
	void render(SDL_Renderer * renderer);

private:

	AttackMessager * attackMessager;
	void processAndPopNextMessage();

	Player * player;
	Map * middleMap;
	Map * bgMap;
	// rectangle to represent the camera
	int camX, camY;

	std::vector<Shape*> mapCollisions;

	// entity vectors
	std::vector<Enemy*> enemies;

	// helper methods
	void updateCameraPosition(Uint32 delta);
	void renderPlayerHealth();
	std::vector<std::string> split(std::string str, char delimiter);

	// static textures for rendering
	std::unique_ptr<Texture> healthTexture;

	std::string mapFile;
	void loadFromFile(std::string path);

	enum MiniState {
		NORMAL = 0,
		INTRO = 1,
		DEATH = 2,
		COMPLETE = 3
	};

	// 0: NORMAL
	// 1: INTRO
	// 2: STAGE COMPLETED
	int miniState;

	// fading variables
	Texture * black;
	int introTimer, introStart;
	std::vector<int> introFadeTimes;
	int currentFade;
	int currentFadeTime;

	// screen shake variables
	int shakeTimer;
	int camOffsetX, camOffsetY;
	int intensity;

	// death transition variables
	Texture * red;
	Texture * deathText;
	Texture * nextStateText;
	int deathStartTime;
	int deathSlowMoFrameCounter;
	int numSlowMoFrames;

	// keep track of winning the level
	int numEnemies, killedEnemies;
	
	// beat level variables
	int levelBeatTime;
};