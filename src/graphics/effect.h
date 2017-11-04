#pragma once

#include <string>
#include <memory>

#include "QcEngine.h"

#define EFFECT_SWITCH_TIME 30

class Effect : public Entity{

public:
	Effect(int x, int y, int frames, int width, int height, std::string path, SDL_Renderer * renderer);
	~Effect();

	void initAtlasTexture();
	void update(float delta);
	void render(SDL_Renderer * renderer);

	void setCamX(int x);
	void setCamY(int y);

	bool REMOVE;

private:
	std::unique_ptr<Texture> texture;

	int currentFrame, maxFrames;
	int width, height;
	int xpos, ypos;
	int camX, camY;

	int effectTimer;
};