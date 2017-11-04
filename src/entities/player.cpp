#include "player.h"

Player::Player(SDL_Renderer * renderer, std::vector<Shape*>* inputMap) {
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
	float yVelocity = .0f;
	faceRight = true;
	onGround = false;
	jumpTimer = 0;
	// set the collision map
	collisionMap = inputMap;
	// set the players own collision box
	collisionBox = Rectangle(position.x, position.y, 64, 64);
}

Player::~Player() {

}

void Player::move(int dir) {
	// 0 is right, 1 is left
	if (dir == 0) {
		moveRight = true;
		faceRight = true;
	} else {
		moveLeft = true;
		faceRight = false;
	}
}

void Player::jump() {
	if (jumpTimer == 0) {
		yVelocity = JUMP_HEIGHT;
		jumpTimer = JUMP_COOLDOWN;
	}
}

void Player::render(SDL_Renderer * renderer) {
	// overload render to account for camera
	int newX = position.x - camX;
	int newY = position.y - camY;
	texture->render(renderer, newX, newY);
}

void Player::update(float delta) {
	// update the jump timer
	if (jumpTimer > 0) {
		if (delta > jumpTimer) {
			jumpTimer = 0;
		}
		else {
			jumpTimer -= static_cast<Uint32>(delta);
		}
	}
	// move according to movement flags
	if (moveRight) {
		moveHelp(1, static_cast<int>(SPEED * delta / 1000));
	}
	if (moveLeft) {
		moveHelp(3, static_cast<int>(SPEED * delta / 1000));
	}
	yVelocity -= GRAVITY * delta / 1000;
	yVelocity = clamp(yVelocity, -20.f, 20.f);
	if (yVelocity < 0) {
		moveHelp(2, static_cast<int>(-yVelocity));
	}
	else {
		moveHelp(0, static_cast<int>(yVelocity));
	}
	onGround = detectOnGround();
	if (onGround) {
		if (moveRight) {
			currentAnimation = MOVE_RIGHT;
		}
		else if (moveLeft) {
			currentAnimation = MOVE_LEFT;
		}
		else if (faceRight) {
			currentAnimation = IDLE_RIGHT;
		}
		else {
			currentAnimation = IDLE_LEFT;
		}
	}
	else {
		if (faceRight) {
			currentAnimation = JUMP_RIGHT;
		}
		else {
			currentAnimation = JUMP_LEFT;
		}
	}
	// update animation logic
	// update the animation frames
	if (currentFrame > animations[currentAnimation].end) {
		currentFrame = animations[currentAnimation].start;
	}
	texture->changeCurrentAtlasTexture(currentFrame);
	currentFrame++;
	// reset the move flags at the end of each state
	moveRight = false, moveLeft = false;
}

void Player::setCamX(int x) {
	camX = x;
}

void Player::setCamY(int y) {
	camY = y;
}

void Player::setupAtlas() {
	// rows
	for (int i = 0; i < 6; i++) {
		// columns
		for (int j = 0; j < 1; j++) {
			texture->getAtlas().emplace_back(Rectangle(j * 64, i * 64, 64, 64));
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

void Player::moveHelp(int dir, int amount) {
	// 0 - Up
	// 1 - Right
	// 2 - Down
	// 3 - Left
	int newX = collisionBox.x;
	int newY = collisionBox.y;
	if (dir == 0) {
		newY -= amount;
	}
	if (dir == 1) {
		newX += amount;
	}
	if (dir == 2) {
		newY += amount;
	}
	if (dir == 3) {
		newX -= amount;
	}
	Rectangle newRect = Rectangle(newX, newY, collisionBox.w, collisionBox.h);
	for (Shape* obj : *collisionMap) {
		while (isColliding(newRect, *obj)) {
			if (dir == 0) {
				newY++;
			}
			if (dir == 1) {
				newX--;
			}
			if (dir == 2) {
				newY--;
			}
			if (dir == 3) {
				newX++;
			}
			newRect.x = newX;
			newRect.y = newY;
		}
	}
	// update the positions in the end
	position.x = newX;
	position.y = newY;
	collisionBox.x = newX;
	collisionBox.y = newY;
}

bool Player::detectOnGround() {
	Rectangle newRect = Rectangle(collisionBox.x, collisionBox.y + 1, collisionBox.w, collisionBox.h);
	for (Shape* obj : *collisionMap) {
		if (isColliding(newRect, *obj)) {
			return true;
		}
	}
	return false;
}