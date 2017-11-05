#pragma once

#include <vector>

#include "QcEngine.h"
#include "../constants.h"
#include "../attackMessage.h"
#include "../entities/player.h"
#include "../graphics/effect.h"

#define NUM_ANIMATIONS_ENEMY 13
#define ENEMY_HEALTH 3
#define MIN_ACTION_TIME 1000
#define ENEMY_SPEED 200
#define REMOVE_TIMER 500
#define ATTACK_CHARGE 1000
#define ATTACK_TIMER 300

class Enemy : public Entity {

	// data structures and array to hold animation info
	struct AnimationInterval {
		unsigned int start;
		unsigned int end;
	};
	AnimationInterval animations[NUM_ANIMATIONS_ENEMY];

	enum animStates {
		IDLE_RIGHT = 0,
		IDLE_LEFT = 1,
		MOVE_RIGHT = 2,
		MOVE_LEFT = 3,
		DEATH_RIGHT = 4,
		DEATH_LEFT = 5,
		BLANK = 6,
		ATTACK_CHARGING_RIGHT = 7,
		ATTACK_CHARGING_LEFT = 8,
		ATTACK_CHARGE_RIGHT = 9,
		ATTACK_CHARGE_LEFT = 10,
		ATTACK_RIGHT = 11,
		ATTACK_LEFT = 12,
	};
	bool statePlayOnce[NUM_ANIMATIONS_ENEMY];
	int nextAnim;

	enum Action {
		ACTION_IDLE = 0,
		ACTION_MOVE_RIGHT = 1,
		ACTION_MOVE_LEFT = 2,
		ACTION_ATTACK_RIGHT = 3,
		ACTION_ATTACK_LEFT = 4,
		ACTION_CHARGE_RIGHT = 5,
		ACTION_CHARGE_LEFT = 6
	};

public:
	Enemy(int x, int y, std::vector<Shape*>* inputMap, SDL_Renderer * renderer, AttackMessager * messager, Player * playerRef);
	~Enemy();

	void update(float delta);
	void render(SDL_Renderer * renderer);
	void updateSimple(float delta);

	void setCamX(int x);
	void setCamY(int y);

	void attack(bool right);
	void takeDamage(int dmg);
	bool DEAD;
	bool REMOVE;
	int removeTimer;

	Rectangle collisionBox;

private:
	AttackMessager * attackMessager;
	// store a reference to the player for attacking stuff
	Player * player;

	int camX, camY;
	std::vector<Shape*>* collisionMap;

	int health;
	int chargeTimer;
	int attackTimer;

	// helper functions to reduce clutter in constructor
	void setupAtlas();
	void setupAnimations();

	int currentAnimation;
	unsigned int currentFrame;
	int frameCounter;

	// helper function to change animation state
	void changeAnimation(int newAnimation);

	float yVelocity;

	bool faceRight;
	bool onGround;

	// function to help with movement
	void moveHelp(int dir, int amount);
	bool detectOnGround();

	int actionTime;
	Action action;

	// keep track of effects and handle rendering them
	std::vector<Effect*> effects;
	SDL_Renderer * renderer;
};