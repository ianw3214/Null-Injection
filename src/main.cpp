#include <iostream>

#include "QcEngine.h"
#include "constants.h"
#include "states\transition.h"
#include "states/menu.h"
#include "states/game.h"
#include "audio\audio.h"

int main(int argc, char* argv[]) {
	
	Engine engine;

	engine.createWindow("Null Injection", SCREEN_WIDTH, SCREEN_HEIGHT, false);

	// initialize audio library
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
		ERR("SDL_Mixer could not initialize...");
		LOG(Mix_GetError());
		return false;
	}

	// start playing the menu music
	// Audio::mute();
	Audio::playTrack("assets/music/menu.wav", 0, true);
	showCursor(false);
	engine.setState(new Transition(new Menu(), 3000, "assets/logo.png", 190, 190, 1000, 1000));
	// engine.setState(new Game("maps/002.txt", true, -1));

	while (engine.isRunning()) {
		engine.update();
	}

	// quit SDL_mixer
	Mix_CloseAudio();

	return 0;

}