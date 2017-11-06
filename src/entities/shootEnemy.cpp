#include "shootEnemy.h"

ShootEnemy::ShootEnemy(int x, int y, std::vector<Shape*>* inputMap, SDL_Renderer * renderer, AttackMessager * messager, Player * playerRef) : Enemy(x, y, inputMap, renderer, messager, playerRef) {
	createTexture("assets/shootEnemy.png", renderer);
	// set up animation data again
	setupAnimations();
	setupAtlas();
	texture->changeCurrentAtlasTexture(0);
	isShootEnemy = true;
}

ShootEnemy::~ShootEnemy() {
	for (Projectile p : projectiles) {
		delete p.texture;
	}
}

void ShootEnemy::attack(bool right) {
	faceRight = right;
	action = right ? ACTION_ATTACK_RIGHT : ACTION_ATTACK_LEFT;
	changeAnimation(right ? ATTACK_RIGHT : ATTACK_LEFT);
	nextAnim = right ? IDLE_RIGHT : IDLE_LEFT;
	attackTimer = ATTACK_TIMER;
	// spawn a projectile
	Projectile projectile;
	projectile.texture = new Texture("assets/projectile.png", renderer);
	projectile.collision = Circle(0, 0, 20);
	projectile.x = position.x;
	projectile.y = position.y;
	// calculate the angle
	projectile.angle = atan2(static_cast<double>(player->getY() - position.y), static_cast<double>(player->getX() - position.x));
	LOG(projectile.angle);
	projectiles.push_back(projectile);
}

void ShootEnemy::update(float delta) {
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
			if (shouldAttackTimer <= 0) {
				bool right = player->getX() > position.x;
				shouldAttackTimer = SHOOT_ATTACK_INTERVAL;
				action = right ? ACTION_CHARGE_RIGHT : ACTION_CHARGE_LEFT;
				chargeTimer = ATTACK_CHARGE;
				changeAnimation(right ? ATTACK_CHARGING_RIGHT : ATTACK_CHARGING_LEFT);
				nextAnim = right ? ATTACK_CHARGE_RIGHT : ATTACK_CHARGE_LEFT;
			}
			else {
				shouldAttackTimer -= static_cast<int>(delta);
			}
		}
		// update action counter
		if (actionTime > 0) {
			actionTime -= static_cast<int>(delta);
		}
		else {
			// update action if we aren't attacking
			if ((action != ACTION_ATTACK_RIGHT) && (action != ACTION_ATTACK_LEFT) && (action != ACTION_CHARGE_RIGHT) && (action != ACTION_CHARGE_LEFT)) {
				action = (Action)randomNumber(3);
				actionTime = MIN_ACTION_TIME + randomNumber(2000);
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
	// update projectiles
	updateProjectiles(delta);
}


void ShootEnemy::render(SDL_Renderer * renderer) {
	// overload render to account for camera
	int newX = position.x - camX;
	int newY = position.y - camY;
	if (texture) texture->render(renderer, newX, newY);
	// render effects
	for (Effect * e : effects) {
		e->render(renderer);
	}
}

void ShootEnemy::renderProjectiles(SDL_Renderer * renderer, int camX, int camY) {
	for (Projectile p : projectiles) {
		p.texture->render(renderer, p.x - camX, p.y - camY);
	}
}


void ShootEnemy::updateProjectiles(float delta) {
	for (int i = projectiles.size() - 1; i >= 0; --i) {
		// update movement
		Projectile p = projectiles.at(i);
		float hypotenuse = static_cast<float>(PROJECTILE_SPEED * delta / 1000);
		int desiredX = static_cast<int>(hypotenuse * cos(p.angle));
		int desiredY = static_cast<int>(hypotenuse * sin(p.angle));
		projectiles.at(i).x += desiredX;
		projectiles.at(i).y += desiredY;
		projectiles.at(i).collision.x = projectiles.at(i).x + 20;
		projectiles.at(i).collision.y = projectiles.at(i).y + 20;
		// send an attack message
		// create an attack message and send it to the messager
		Circle * circle = new Circle(projectiles.at(i).collision);
		AttackMessage message = AttackMessage(PLAYER, 1, circle);
		message.recoilRight = faceRight;
		attackMessager->addMessage(message);
		// check for collisions with environment or player
		if (isColliding(projectiles.at(i).collision, player->getCollisionBox())) {
			Texture * temp = p.texture;
			projectiles.erase(projectiles.begin() + i);
			delete temp;
			return;
		}
		else {
			for (Shape * obj : *collisionMap) {
				if (isColliding(projectiles.at(i).collision, *obj)) {
					Texture * temp = p.texture;
					projectiles.erase(projectiles.begin() + i);
					delete temp;
					return;
				}
			}
		}
	}
}