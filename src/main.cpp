#include <iostream>

#include "QcEngine.h"
#include "constants.h"
#include "states/game.h"

int main(int argc, char* argv[]) {
	
	Engine engine;

	engine.createWindow("Fusion Blitz", SCREEN_WIDTH, SCREEN_HEIGHT, false);
	engine.setState(new Game());

	while (engine.isRunning()) {
		engine.update();
	}

	return 0;

}