#pragma once

#include <string>

#include "QcEngine.h"
#include "../entities/player.h"

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

};