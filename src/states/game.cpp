#include "game.h"

Game::Game() {

}

Game::~Game() {

}

void Game::init() {
	// hide the cursor
	// showCursor(false);

	// instantiate a new player
	player = new Player(renderer, &mapCollisions);
	entities.push_back(player);

	// setup the map colliisons
	mapCollisions.emplace_back(new Rectangle(0, 400, 500, 50));

	// setup the camera
	camX = 0, camY = 0;
}

void Game::cleanUp() {

}

void Game::update(Uint32 delta) {
	State::update(delta);
	// handle player movement
	if (keyPressed(SDL_SCANCODE_LEFT)) {
		player->move(1);
	}
	if (keyPressed(SDL_SCANCODE_RIGHT)) {
		player->move(0);
	}
	if (keyPressed(SDL_SCANCODE_SPACE)) {
		player->jump();
	}
	// update camera position
	updateCameraPosition(delta);
	// update camera settings of each entity
	player->setCamX(camX);
	player->setCamY(camY);
}

void Game::render(SDL_Renderer * renderer) {
	State::render(renderer);
}

void Game::updateCameraPosition(Uint32 delta) {
	
	if (camX > player->getX() - CAM_MARGIN) {
		camX = player->getX() - CAM_MARGIN;
	}
	/*
	if (camX + SCREEN_WIDTH < player->getX() + CAM_MARGIN) {
		camX = player->getX() + CAM_MARGIN;
	}
	if (camY < player->getY() - CAM_MARGIN) {
		camY = player->getY() - CAM_MARGIN;
	}
	if (camY + SCREEN_HEIGHT > player->getY() + CAM_MARGIN) {
		camY = player->getY() + CAM_MARGIN;
	}
	*/
}