#pragma once

#include "QcEngine.h"

#define NUM_ANIMATIONS 6

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

	Player(SDL_Renderer * renderer);
	~Player();

	void move();

	void update(float delta);

private:

	int currentAnimation;
	int currentFrame;

	bool moveLeft, moveRight;
	bool faceRight;

	// helper functions to reduce clutter in constructor
	void setupAtlas();
	void setupAnimations();

	// helper function to change animation state
	void changeAnimation(int newAnimation);
};