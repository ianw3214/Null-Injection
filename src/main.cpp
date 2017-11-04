#include <iostream>

#include "QcEngine.h"
#include "states/game.h"

int main(int argc, char* argv[]) {
	
	Engine engine;

	engine.createWindow("Fusion Blitz", 500, 500, false);
	engine.setState(new Game());
	
	while (engine.isRunning()) {
		engine.update();
	}

	return 0;

}