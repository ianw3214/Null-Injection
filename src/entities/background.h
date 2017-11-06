#pragma once

#include <vector>
#include <utility>

#include "QcEngine.h"

// types
// -1 - undefined
//	0 - single bit

#define BIT_BASE_SPEED_TOP 500
#define BIT_BASE_SPEED_MID 300
#define BIT_BASE_SPEED_FAR 150

struct element {
	int posX, posY;
	int type;
	int speed;
	element() : posX(0), posY(0), type(-1) {}
	element(int _x, int _y, int _t) : posX(_x), posY(_y), type(_t) {}
};

struct singleBit : public element {
	Texture * texture;
	bool up;
	int speed;
	singleBit() : element(0, 0, 0) {}
	singleBit(int _x, int _y) : element(_x, _y, 0) {}
};

class Background : public Entity {
public:
	Background(SDL_Renderer* renderer);
	~Background();

	void populate();
	void update(float delta);
	void render(SDL_Renderer* renderer, int camX, int camY);

	// MAKE SURE TO SET THESE IN GAME CLASS
	Texture * largeGreen0;
	Texture * largeGreen1;
	Texture * smallGreen0;
	Texture * smallGreen1;
	Texture * tinyGreen0;
	Texture * tinyGreen1;

private:

	// the actual bits in the background
	std::vector<element*> layer1;
	std::vector<element*> layer2;
	std::vector<element*> layer3;

	// bit spawning method
	void spawnBits();
	void spawnBitsCloneWithMapYPositions();

};