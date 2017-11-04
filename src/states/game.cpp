#include "game.h"

Game::Game() {

}

Game::~Game() {

}

void Game::init() {
	// hide the cursor
	// showCursor(false);

	// instantiate a new player
	player = new Player(renderer);
	entities.push_back(player);
}

void Game::cleanUp() {

}

void Game::update(Uint32 delta) {
	State::update(delta);
}

void Game::render(SDL_Renderer * renderer) {
	State::render(renderer);
}
