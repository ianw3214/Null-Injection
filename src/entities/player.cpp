#include "player.h"

Player::Player(SDL_Renderer * renderer, std::vector<Shape*>* inputMap, int _x, int _y, AttackMessager * messager) {
	attackMessager = messager;
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
	frameCounter = 0;
	// set initial movement variables
	moveRight = false, moveLeft = false;
	float yVelocity = .0f;
	faceRight = true;
	onGround = false;
	jumpTimer = 0;
	canJump = false;
	rolling = false;
	rollCoolDown = 0;
	attacking = false;
	attackCoolDown = 0;
	// set the collision map
	collisionMap = inputMap;
	// set the players own collision box
	position.x = _x;
	position.y = _y;
	collisionBox = Rectangle(position.x, position.y, 64, 64);
	// save the renderer in the player class to create effects
	this->renderer = renderer;
	health = PLAYER_HEALTH;
	invincibleTimer = 0;
}

Player::~Player() {
	// remove remaining effects
	for (Effect * e : effects) {
		delete e;
	}
}

void Player::move(int dir) {
	// don't move if rolling
	if (attacking || rolling) return;
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
	// make sure the player can jump before actually jumping
	if (canJump && jumpTimer == 0 && !rolling) {
		yVelocity = JUMP_HEIGHT;
		onGround = false;
		canJump = false;
		jumpTimer = JUMP_COOLDOWN;
	}
}

void Player::roll() {
	// don't roll if attacking or rolling
	if (attacking || rolling) return;
	rollCoolDown = ROLL_TIME;
	rolling = true;
	changeAnimation(faceRight ? ROLL_RIGHT : ROLL_LEFT);
	nextAnim = faceRight ? IDLE_RIGHT : IDLE_LEFT;
	invincibleTimer = INVINCIBLE_TIME;
}

void Player::attack() {
	// don't attack if attacking or rolling
	if (attacking || rolling) return;
	attackCoolDown = ATTACK_TIME;
	attacking = true;
	changeAnimation(faceRight ? ATTACK_RIGHT : ATTACK_LEFT);
	nextAnim = faceRight ? IDLE_RIGHT : IDLE_LEFT;
	int effectX = faceRight ? position.x : position.x - 36;
	std::string effectPath = faceRight ? "assets/attack_effect.png" : "assets/attack_effect_left.png";
	Effect * effect = new Effect(effectX, position.y - 36, 6, 100, 100, effectPath, renderer);
	effect->setCamX(camX);
	effect->setCamY(camY);
	effect->initAtlasTexture();
	effects.push_back(effect);
	// create an attack message and send it to the messager
	AttackMessage message = AttackMessage(ENEMY, 1, new Rectangle(effectX, position.y - 36, 100, 100));
	attackMessager->addMessage(message);
}

void Player::render(SDL_Renderer * renderer) {
	// overload render to account for camera
	int newX = position.x - camX;
	int newY = position.y - camY;
	if (texture) texture->render(renderer, newX, newY);
	// render effects
	for (Effect * e : effects) {
		e->render(renderer);
	}
}

void Player::update(float delta) {
	// update invincible timer
	if (invincibleTimer > 0) {
		invincibleTimer -= static_cast<int>(delta);
		if (invincibleTimer < 0) invincibleTimer = 0;
	}
	// update jump timer
	if (jumpTimer > 0) {
		if (jumpTimer <= delta) jumpTimer = 0;
		else jumpTimer -= static_cast<int>(delta);
	}
	if (attackCoolDown > 0) {
		if (attackCoolDown <= delta) {
			attackCoolDown = 0;
			attacking = false;
		}
		else attackCoolDown -= static_cast<int>(delta);
	}
	// move according to movement flags
	if (rolling) {
		if (rollCoolDown < delta) {
			rollCoolDown = 0;
			rolling = false;
		}
		else rollCoolDown -= static_cast<int>(delta);
		if (faceRight) moveHelp(1, static_cast<int>(SPEED * delta / 1000 * ROLL_SPEED_MODIFIER));
		else moveHelp(3, static_cast<int>(SPEED * delta / 1000 * ROLL_SPEED_MODIFIER));
	} else {
		if (moveRight) {
			moveHelp(1, static_cast<int>(SPEED * delta / 1000));
		}
		if (moveLeft) {
			moveHelp(3, static_cast<int>(SPEED * delta / 1000));
		}
	}
	updateYVelocityIfHittingCeiling();
	if (!onGround) yVelocity -= GRAVITY * delta / 1000;
	else yVelocity = .0f;
	yVelocity = clamp(yVelocity, -20.f, 20.f);
	if (yVelocity < 0) {
		moveHelp(2, static_cast<int>(-yVelocity));
	}
	else {
		moveHelp(0, static_cast<int>(yVelocity));
	}
	// update animations based on move states
	onGround = detectOnGround();
	if (!rolling && !attacking) {
		if (onGround) {
			if (moveRight) {
				changeAnimation(MOVE_RIGHT);
			}
			else if (moveLeft) {
				changeAnimation(MOVE_LEFT);
			}
			else if (faceRight) {
				changeAnimation(IDLE_RIGHT);
			}
			else {
				changeAnimation(IDLE_LEFT);
			}
			// user can jump when on ground
			canJump = true;
		}
		else {
			if (faceRight) {
				changeAnimation(JUMP_RIGHT);
			}
			else {
				changeAnimation(JUMP_LEFT);
			}
		}
	}
	// let the player wall jump
	if (jumpTimer == 0) updateCanJumpIfHittingWall();
	// update animation logic
	// update the animation frames
	if (frameCounter > ANIMATION_INTERVAL) {
		frameCounter = 0;
		currentFrame++;
		if (currentFrame > animations[currentAnimation].end) {
			currentFrame = animations[currentAnimation].start;
			// end the animation if it should be over
			if (statePlayOnce[currentAnimation]) {
				changeAnimation(nextAnim);
			}
		}
		texture->changeCurrentAtlasTexture(currentFrame);
	}
	else {
		frameCounter++;
	}
	// reset the move flags at the end of each state
	moveRight = false, moveLeft = false;
	// update effects
	// REALLY BAD CODE RIGHT HERE
	std::vector<int> removeIndices;
	for (unsigned int i = 0; i < effects.size(); ++i) {
		effects[i]->setCamX(camX);
		effects[i]->setCamY(camY);
		effects[i]->update(delta);
		if (effects[i]->REMOVE) {
			removeIndices.push_back(i);
		}
	}
	std::reverse(removeIndices.begin(), removeIndices.end());
	for (int index : removeIndices) {
		Effect * temp = effects.at(index);
		effects.erase(effects.begin() + index);
		delete temp;
	}
}

void Player::setCamX(int x) {
	camX = x;
}

void Player::setCamY(int y) {
	camY = y;
}

void Player::takeDamage(int dmg) {
	if (invincibleTimer == 0) {
		health -= dmg;
		if (health <= 0) {
			DEAD = true;
		}
		invincibleTimer = INVINCIBLE_TIME;
	}
}

int Player::getHealth() const {
	return health;
}

Rectangle Player::getCollisionBox() const {
	return collisionBox;
}

void Player::setupAtlas() {
	// rows
	for (int i = 0; i < 10; i++) {
		// columns
		for (int j = 0; j < 8; j++) {
			texture->getAtlas().emplace_back(Rectangle(j * 64, i * 64, 64, 64));
		}
	}
}

void Player::setupAnimations() {
	animations[0] = { 0, 0 };
	animations[1] = { 8, 8 };
	animations[2] = { 16, 23 };
	animations[3] = { 24, 31 };
	animations[4] = { 32, 32 };
	animations[5] = { 40, 40 };
	animations[6] = { 48, 55 };
	animations[7] = { 56, 63 };
	animations[8] = { 64, 69 };
	animations[9] = { 72, 77 };
	statePlayOnce[0] = false;
	statePlayOnce[1] = false;
	statePlayOnce[2] = false;
	statePlayOnce[3] = false;
	statePlayOnce[4] = false;
	statePlayOnce[5] = false;
	statePlayOnce[6] = true;
	statePlayOnce[7] = true;
	statePlayOnce[8] = true;
	statePlayOnce[9] = true;
	nextAnim = IDLE_RIGHT;
}

void Player::changeAnimation(int newAnimation) {
	if (newAnimation == currentAnimation) return;
	currentAnimation = newAnimation;
	currentFrame = animations[newAnimation].start;
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

void Player::updateYVelocityIfHittingCeiling() {
	Rectangle newRect = Rectangle(collisionBox.x, collisionBox.y - 1, collisionBox.w, collisionBox.h);
	for (Shape* obj : *collisionMap) {
		if (isColliding(newRect, *obj)) {
			yVelocity = -1.f;
			return;
		}
	}
}

void Player::updateCanJumpIfHittingWall() {
	// the function is irrelavent if the player can jump already
	if (canJump) return;
	// otherwise, detect if the player is beside a wall and let him/her jump if true
	Rectangle newRect1 = Rectangle(collisionBox.x + 1, collisionBox.y, collisionBox.w, collisionBox.h);
	Rectangle newRect2 = Rectangle(collisionBox.x - 1, collisionBox.y, collisionBox.w, collisionBox.h);
	for (Shape* obj : *collisionMap) {
		if (isColliding(newRect1, *obj)) {
			canJump = true;
			return;
		}
		if (isColliding(newRect2, *obj)) {
			canJump = true;
			return;
		}
	}
}