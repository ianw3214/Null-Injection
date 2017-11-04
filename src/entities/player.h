#pragma once

#include <vector>

#include "QcEngine.h"

#define NUM_ANIMATIONS 6
#define GRAVITY 20
#define SPEED 300
#define JUMP_COOLDOWN 1000
#define JUMP_HEIGHT 8.f

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
		JUMP_LEFT = 5
	};

public:

	Player(SDL_Renderer * renderer, std::vector<Shape*>* inputMap);
	~Player();

	void move(int dir);
	void jump();

	void render(SDL_Renderer * renderer);
	void update(float delta);

	void setCamX(int x);
	void setCamY(int y);

private:

	int camX, camY;

	Rectangle collisionBox;

	int currentAnimation;
	unsigned int currentFrame;

	bool moveLeft, moveRight;
	float yVelocity;
	bool faceRight;
	bool onGround;
	Uint32 jumpTimer;

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
};