#include "enemy.h"
#include "../audio/audio.h"

Enemy::Enemy(int x, int y, std::vector<Shape*>* inputMap, SDL_Renderer * renderer, AttackMessager * messager, Player * playerRef) {
	player = playerRef;
	attackMessager = messager;
	createTexture("assets/enemy.png", renderer);
	position.x = x;
	position.y = y;
	collisionBox.x = x;
	collisionBox.y = y;
	collisionMap = inputMap;
	health = ENEMY_HEALTH;
	DEAD = false, REMOVE = false;
	removeTimer = REMOVE_TIMER;
	attackTimer = 0, chargeTimer = 0;
	collisionBox = Rectangle(x, y, 64, 64);
	yVelocity = 0.f;
	onGround = false;
	actionTime = 0;
	faceRight = true;
	action = ACTION_IDLE;
	currentAnimation = IDLE_RIGHT;
	currentFrame = 0;
	frameCounter = 0;
	// set up animation data
	setupAnimations();
	setupAtlas();
	texture->changeCurrentAtlasTexture(0);
	this->renderer = renderer;
	isShootEnemy = false;
}

Enemy::~Enemy() {

}

void Enemy::update(float delta) {
	if (!DEAD) {
		// update movement as well as animations
		if (action == ACTION_CHARGE_RIGHT) {
			faceRight = true;
			chargeTimer -= static_cast<int>(delta);
			if (chargeTimer <= 0) {
				chargeTimer = 0;
				attack(true);
			}
		}
		else if (action == ACTION_CHARGE_LEFT) {
			faceRight = false;
			chargeTimer -= static_cast<int>(delta);
			if (chargeTimer <= 0) {
				chargeTimer = 0;
				attack(false);
			}
		}
		else if (action == ACTION_ATTACK_RIGHT) {
			faceRight = true;
			attackTimer -= static_cast<int>(delta);
			if (attackTimer <= 0) {
				attackTimer = 0;
				action = ACTION_IDLE;
			}
		}
		else if (action == ACTION_ATTACK_LEFT) {
			faceRight = false;
			attackTimer -= static_cast<int>(delta);
			if (attackTimer <= 0) {
				attackTimer = 0;
				action = ACTION_IDLE;
			}
		} 
		else if (action == ACTION_IDLE) {
			if (faceRight) changeAnimation(IDLE_RIGHT);
			else changeAnimation(IDLE_LEFT);
		}
		else if (action == ACTION_MOVE_RIGHT) {
			faceRight = true;
			changeAnimation(MOVE_RIGHT);
			moveHelp(1, static_cast<int>(ENEMY_SPEED * delta / 1000));
		}
		else if (action == ACTION_MOVE_LEFT) {
			faceRight = false;
			changeAnimation(MOVE_LEFT);
			moveHelp(3, static_cast<int>(ENEMY_SPEED * delta / 1000));
		}
		// calculate recoils
		if (recoilTimer > 0) {
			recoilTimer -= static_cast<int>(delta);
			if (recoilTimer < 0) recoilTimer = 0;
			float percentage = static_cast<float>(recoilTimer) / static_cast<float>(RECOIL_BAR);
			moveHelp(recoilFaceRight ? 1 : 3, static_cast<int>(RECOIL_SPEED * delta / 1000 * percentage));
		}
		// check if we should attack if we aren't already
		if ((action != ACTION_ATTACK_RIGHT) && (action != ACTION_ATTACK_LEFT) && (action != ACTION_CHARGE_RIGHT) && (action != ACTION_CHARGE_LEFT)) {
			// update action counter
			if (actionTime > 0) {
				actionTime -= static_cast<int>(delta);
			}
			else {
				action = (Action)randomNumber(3);
				actionTime = MIN_ACTION_TIME + randomNumber(2000);
			}
			// check to see if we should attack
			Line leftCollision = Line(position.x - 200, position.y + 32, position.x, position.y + 32);
			if (isColliding(leftCollision, player->getCollisionBox())) {
				action = ACTION_CHARGE_LEFT;
				chargeTimer = ATTACK_CHARGE;
				changeAnimation(ATTACK_CHARGING_LEFT);
				nextAnim = ATTACK_CHARGE_LEFT;
			}
			Line rightCollision = Line(position.x + 64, position.y + 32, position.x + 264, position.y + 32);
			if (isColliding(rightCollision, player->getCollisionBox())) {
				action = ACTION_CHARGE_RIGHT;
				chargeTimer = ATTACK_CHARGE;
				changeAnimation(ATTACK_CHARGING_RIGHT);
				nextAnim = ATTACK_CHARGE_RIGHT;
			}
		}
		// update y velocity as well
		if (!onGround) yVelocity -= GRAVITY * delta / 1000;
		else yVelocity = 0.0f;
		yVelocity = clamp(yVelocity, -20.f, 20.f);
		if (yVelocity < 0) {
			moveHelp(2, static_cast<int>(-yVelocity));
		}
		else {
			moveHelp(0, static_cast<int>(yVelocity));
		}
		onGround = detectOnGround();
		// update enemy collision box
		collisionBox.x = position.x;
		collisionBox.y = position.y;
	}
	else {
		removeTimer -= static_cast<int>(delta);
		if (removeTimer <= 0) REMOVE = true;
	}
	// update animation frames
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

// update function used for after player has died
void Enemy::updateSimple(float delta) {
	// update movement as well as animations
	if (action == ACTION_CHARGE_RIGHT) {
		faceRight = true;
		chargeTimer = 0;
		action = ACTION_IDLE;
	}
	else if (action == ACTION_CHARGE_LEFT) {
		faceRight = false;
		chargeTimer = 0;
		action = ACTION_IDLE;
	}
	else if (action == ACTION_ATTACK_RIGHT) {
		faceRight = true;
		attackTimer = 0;
		action = ACTION_IDLE;
	}
	else if (action == ACTION_ATTACK_LEFT) {
		faceRight = false;
		attackTimer = 0;
		action = ACTION_IDLE;
	}
	else if (action == ACTION_IDLE) {
		if (faceRight) changeAnimation(IDLE_RIGHT);
		else changeAnimation(IDLE_LEFT);
	}
	else if (action == ACTION_MOVE_RIGHT) {
		faceRight = true;
		changeAnimation(MOVE_RIGHT);
		moveHelp(1, static_cast<int>(ENEMY_SPEED * delta / 1000));
	}
	else if (action == ACTION_MOVE_LEFT) {
		faceRight = false;
		changeAnimation(MOVE_LEFT);
		moveHelp(3, static_cast<int>(ENEMY_SPEED * delta / 1000));
	}
	// update y velocity as well
	if (!onGround) yVelocity -= GRAVITY * delta / 1000;
	else yVelocity = 0.0f;
	yVelocity = clamp(yVelocity, -20.f, 20.f);
	if (yVelocity < 0) {
		moveHelp(2, static_cast<int>(-yVelocity));
	}
	else {
		moveHelp(0, static_cast<int>(yVelocity));
	}
	onGround = detectOnGround();
	// update enemy collision box
	collisionBox.x = position.x;
	collisionBox.y = position.y;
	// update animation frames
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

void Enemy::render(SDL_Renderer * renderer) {
	// overload render to account for camera
	int newX = position.x - camX;
	int newY = position.y - camY;
	if (texture) texture->render(renderer, newX, newY);
	// render effects
	for (Effect * e : effects) {
		e->render(renderer);
	}
}

void Enemy::setCamX(int x) {
	camX = x;
}

void Enemy::setCamY(int y) {
	camY = y;
}

void Enemy::attack(bool right) {
	faceRight = right;
	action = right ? ACTION_ATTACK_RIGHT : ACTION_ATTACK_LEFT;
	changeAnimation(right ? ATTACK_RIGHT : ATTACK_LEFT);
	nextAnim = right ? IDLE_RIGHT : IDLE_LEFT;
	attackTimer = ATTACK_TIMER;
	// EFFECT CODE
	int effectX = right? position.x + 64 : position.x - 200;
	std::string effectPath = right ? "assets/enemy_attack.png" : "assets/enemy_attack.png";
	Effect * effect = new Effect(effectX, position.y, 10, 200, 64, effectPath, renderer);
	effect->setCamX(camX);
	effect->setCamY(camY);
	effect->initAtlasTexture();
	effects.push_back(effect);
	// create an attack message and send it to the messager
	AttackMessage message = AttackMessage(PLAYER, 1, new Rectangle(effectX, position.y, 200, 64));
	message.recoilRight = faceRight;
	attackMessager->addMessage(message);
	// play the laser sound
	Audio::playTrack("assets/sfx/laser.wav", 1, false);
}

bool Enemy::takeDamage(int dmg, bool right) {
	if (DEAD) return false;
	// if we are being recoiled, don't take damage
	if (recoilTimer > 0) return false;
	health -= dmg;
	recoilTimer = RECOIL_BAR;
	recoilFaceRight = right;
	if (health <= 0) {
		DEAD = true;
		changeAnimation(faceRight ? DEATH_RIGHT : DEATH_LEFT);
		nextAnim = BLANK;
		return true;
	}
	return false;
}

void Enemy::setupAtlas() {
	// rows
	for (int i = 0; i < 10; i++) {
		// columns
		for (int j = 0; j < 8; j++) {
			texture->getAtlas().emplace_back(Rectangle(j * 64, i * 64, 64, 64));
		}
	}
}

void Enemy::setupAnimations() {
	animations[0] = { 0, 0 };
	animations[1] = { 8, 8 };
	animations[2] = { 16, 23 };
	animations[3] = { 24, 31 };
	animations[4] = { 32, 36 };
	animations[5] = { 40, 44 };
	animations[6] = { 45, 45 };
	animations[7] = { 48, 52 };
	animations[8] = { 56, 60 };
	animations[9] = { 52, 52 };
	animations[10] = { 60, 60 };
	animations[11] = { 64, 71 };
	animations[12] = { 72, 79 };
	statePlayOnce[0] = false;
	statePlayOnce[1] = false;
	statePlayOnce[2] = false;
	statePlayOnce[3] = false;
	statePlayOnce[4] = true;
	statePlayOnce[5] = true;
	statePlayOnce[6] = false;
	statePlayOnce[7] = true;
	statePlayOnce[8] = true;
	statePlayOnce[9] = false;
	statePlayOnce[10] = false;
	statePlayOnce[11] = true;
	statePlayOnce[12] = true;
	nextAnim = IDLE_RIGHT;
}

void Enemy::changeAnimation(int newAnimation) {
	if (newAnimation == currentAnimation) return;
	currentAnimation = newAnimation;
	currentFrame = animations[newAnimation].start;
}

void Enemy::moveHelp(int dir, int amount) {
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
			// check if up margins can work
			if (dir == 0) {
				newY++;
			}
			if (dir == 1) {
				// check if we can move with up margin
				Rectangle checkMargin = Rectangle(newX, newY - ENEMY_UP_MARGIN, collisionBox.w, collisionBox.h);
				if (!isColliding(checkMargin, *obj)) {
					while (!isColliding(checkMargin, *obj)) {
						checkMargin.y++;
					}
					newY = --checkMargin.y;
				}
				else {
					newX--;
				}
			}
			if (dir == 2) {
				newY--;
			}
			if (dir == 3) {
				// check if we can move with up margin
				Rectangle checkMargin = Rectangle(newX, newY - ENEMY_UP_MARGIN, collisionBox.w, collisionBox.h);
				if (!isColliding(checkMargin, *obj)) {
					while (!isColliding(checkMargin, *obj)) {
						checkMargin.y++;
					}
					newY = --checkMargin.y;
				}
				else {
					newX++;
				}
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

bool Enemy::detectOnGround() {
	Rectangle newRect = Rectangle(collisionBox.x, collisionBox.y + 1, collisionBox.w, collisionBox.h);
	for (Shape* obj : *collisionMap) {
		if (isColliding(newRect, *obj)) {
			return true;
		}
	}
	return false;
}