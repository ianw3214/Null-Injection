#include "effect.h"

Effect::Effect(int x, int y, int frames, int width, int height, std::string path, SDL_Renderer * renderer) {
	// start 1 before the first frame because updates are called before renders
	currentFrame = 0;
	maxFrames = frames;
	this->width = width;
	this->height = height;
	this->texture = std::unique_ptr<Texture>(new Texture(path, renderer));
	REMOVE = false;
	// setup the atlas
	for (int i = 0; i < frames; i++) {
		texture->getAtlas().push_back(Rectangle(i * width, 0, width, height));
	}
	effectTimer = EFFECT_SWITCH_TIME;
	xpos = x;
	ypos = y;
}

Effect::~Effect() {

}

void Effect::initAtlasTexture() {
	texture->changeCurrentAtlasTexture(0);
}

void Effect::update(float delta) {
	if (effectTimer <= 0) {
		currentFrame++;
		if (currentFrame >= maxFrames) {
			currentFrame -= 1;
			REMOVE = true;
		}
		texture->changeCurrentAtlasTexture(currentFrame);
		effectTimer = EFFECT_SWITCH_TIME;
	}
	else {
		effectTimer -= static_cast<int>(delta);
	}
}

void Effect::render(SDL_Renderer * renderer) {
	if (texture) texture->render(renderer, xpos - camX, ypos - camY);
}

void Effect::setCamX(int x) {
	camX = x;
}

void Effect::setCamY(int y) {
	camY = y;
}
