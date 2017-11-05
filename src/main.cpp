#include <iostream>

#include "QcEngine.h"
#include "constants.h"
#include "states/menu.h"
#include "states/game.h"
#include "SDL_mixer.h"

int main(int argc, char* argv[]) {
	
	Engine engine;

	engine.createWindow("Fusion Blitz", SCREEN_WIDTH, SCREEN_HEIGHT, false);

	// initialize audio library
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
		ERR("SDL_Mixer could not initialize...");
		LOG(Mix_GetError());
		return false;
	}

	engine.setState(new Menu());

	while (engine.isRunning()) {
		engine.update();
	}

	// quit SDL_mixer
	Mix_CloseAudio();

	return 0;

}