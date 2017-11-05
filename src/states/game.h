#pragma once

#include <vector>
#include <algorithm>
#include <memory>

#include "QcEngine.h"
#include "../attackMessage.h"
#include "../constants.h"
#include "../entities/player.h"
#include "../entities/enemy.h"
#include "../entities/map.h"
#include "../audio/audio.h"

#define CAM_SPEED 300
#define CAM_MARGIN 100
#define HEALTH_MARGIN_Y 20
#define HEALTH_MARGIN_X 10

class Game : public State {

public:
	Game();
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

	// static textures for rendering
	std::unique_ptr<Texture> healthTexture;
};