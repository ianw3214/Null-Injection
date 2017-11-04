#include "game.h"

Game::Game() {

}

Game::~Game() {
	delete middleMap;
}

void Game::init() {
	// hide the cursor
	// showCursor(false);

	// instantiate a new player
	player = new Player(renderer, &mapCollisions, 500, 500);
	entities.push_back(player);

	// setup the map colliisons
	mapCollisions.emplace_back(new Rectangle(0, 1024, 1024, 50));
	mapCollisions.emplace_back(new Rectangle(0, 768, 448, 128));
	mapCollisions.emplace_back(new Rectangle(576, 704, 448, 64));
	mapCollisions.emplace_back(new Rectangle(768, 576, 256, 128));
	mapCollisions.emplace_back(new Rectangle(256, 384, 448, 128));
	mapCollisions.emplace_back(new Rectangle(0, 192, 192, 128));

	// setup the camera
	camX = 0, camY = 0;

	// setup the map
	middleMap = new Map("assets/map.png", renderer);
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
	if (keyPressed(SDL_SCANCODE_X)) {
		player->roll();
	}
	if (keyPressed(SDL_SCANCODE_Z)) {
		player->attack();
	}
	// update camera position
	updateCameraPosition(delta);
	// update camera settings of each entity
	player->setCamX(camX);
	player->setCamY(camY);
}

void Game::render(SDL_Renderer * renderer) {
	// render the map first
	middleMap->render(renderer, camX, camY);
	State::render(renderer);
}

void Game::updateCameraPosition(Uint32 delta) {
	if (camX > player->getX() - CAM_MARGIN) {
		camX = player->getX() - CAM_MARGIN;
	}
	if (camX + SCREEN_WIDTH < player->getX() + 64 + CAM_MARGIN) {
		camX = player->getX() + 64 + CAM_MARGIN - SCREEN_WIDTH;
	}
	if (camY > player->getY() - CAM_MARGIN) {
		camY = player->getY() - CAM_MARGIN;
	}
	if (camY + SCREEN_HEIGHT < player->getY() + 64 + CAM_MARGIN) {
		camY = player->getY() + 64 + CAM_MARGIN - SCREEN_HEIGHT;
	}
}