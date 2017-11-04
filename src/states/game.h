#pragma once

#include <vector>

#include "QcEngine.h"
#include "../constants.h"
#include "../entities/player.h"
#include "../entities/map.h"

#define CAM_SPEED 300
#define CAM_MARGIN 100

class Game : public State {

public:
	Game();
	~Game();

	void init();
	void cleanUp();

	void update(Uint32 delta);
	void render(SDL_Renderer * renderer);

private:

	Player * player;
	Map * middleMap;
	// rectangle to represent the camera
	int camX, camY;

	std::vector<Shape*> mapCollisions;

	// helper methods
	void updateCameraPosition(Uint32 delta);
};