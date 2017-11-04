#include "player.h"

Player::Player(SDL_Renderer * renderer) {
	// create the player texture
	createTexture("assets/player.png", renderer);
	// setup atlas/spritesheet of player
	setupAtlas();
	setupAnimations();
	// set an initial animation state
	texture->changeCurrentAtlasTexture(IDLE_RIGHT);
	// set initial variables for animations
	currentAnimation = 0;
	currentFrame = 0;
	// set initial movement variables
	moveRight = false, moveLeft = false;
	faceRight = true;
}

Player::~Player() {

}

void Player::move() {

}

void Player::update(float delta) {

}

void Player::setupAtlas() {
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 1; j++) {
			texture->getAtlas().emplace_back(Rectangle(j * 32, i * 32, (j + 1) * 32, (i + 1) * 32));
		}
	}
}

void Player::setupAnimations() {
	animations[0] = { 0, 0 };
	animations[1] = { 1, 1 };
	animations[2] = { 2, 2 };
	animations[3] = { 3, 3 };
	animations[4] = { 4, 4 };
	animations[5] = { 5, 5 };
}

void Player::changeAnimation(int newAnimation) {
	if (newAnimation == currentAnimation) return;
	currentAnimation = newAnimation;
	currentFrame = animations[currentAnimation].start;
}