#pragma once

#include <vector>
#include <algorithm>

#include "QcEngine.h"
#include "../attackMessage.h"
#include "../graphics/effect.h"
#include "../constants.h"

#define NUM_ANIMATIONS 10
#define SPEED 300
#define JUMP_HEIGHT 8.f
#define JUMP_COOLDOWN 300
#define ROLL_TIME 450
#define ROLL_SPEED_MODIFIER 2
#define ATTACK_TIME 300
#define PLAYER_HEALTH 5
#define INVINCIBLE_TIME 300

class Player : public Entity {

	// data structures and array to hold animation info
	struct AnimationInterval {
		unsigned int start;
		unsigned int end;
	};
	AnimationInterval animations[NUM_ANIMATIONS];

	enum animStates {
		IDLE_RIGHT = 0,
		IDLE_LEFT = 1,
		MOVE_RIGHT = 2,
		MOVE_LEFT = 3,
		JUMP_RIGHT = 4,
		JUMP_LEFT = 5,
		ROLL_RIGHT = 6,
		ROLL_LEFT = 7,
		ATTACK_RIGHT = 8,
		ATTACK_LEFT = 9
	};
	bool statePlayOnce[NUM_ANIMATIONS];
	int nextAnim;

public:

	Player(SDL_Renderer * renderer, std::vector<Shape*>* inputMap, int _x, int _y, AttackMessager * messager);
	~Player();

	void move(int dir);
	void jump();
	void roll();
	void attack();

	void render(SDL_Renderer * renderer);
	void update(float delta);

	void setCamX(int x);
	void setCamY(int y);

	void takeDamage(int dmg);
	int getHealth() const;

	Rectangle getCollisionBox() const;

private:

	AttackMessager * attackMessager;
	bool DEAD;
	int health;

	int camX, camY;

	Rectangle collisionBox;

	int currentAnimation;
	unsigned int currentFrame;
	int frameCounter;

	bool moveLeft, moveRight;
	float yVelocity;
	bool faceRight;
	bool onGround;
	Uint32 jumpTimer;
	bool canJump;
	bool rolling;
	Uint32 rollCoolDown;
	bool attacking;
	Uint32 attackCoolDown;
	int invincibleTimer;

	// helper functions to reduce clutter in constructor
	void setupAtlas();
	void setupAnimations();

	// helper function to change animation state
	void changeAnimation(int newAnimation);

	// pointer to the collision objects to calculate movement
	std::vector<Shape*>* collisionMap;

	// function to help with movement
	void moveHelp(int dir, int amount);

	// other helper methods
	bool detectOnGround();
	void updateYVelocityIfHittingCeiling();
	void updateCanJumpIfHittingWall();

	// keep track of effects and handle rendering them
	std::vector<Effect*> effects;
	SDL_Renderer * renderer;
};