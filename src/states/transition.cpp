#include "transition.h"
#include "../audio/audio.h"

Transition::Transition(State * next, Uint32 duration, std::string texturePath, int textureX, int textureY, int fIn, int fOut) {
	path = texturePath;
	x = textureX;
	y = textureY;
	this->next = next;
	this->duration = duration;
	fadeIn = fIn, fadeOut = fOut;
}

Transition::~Transition() {
	delete texture;
	delete black;
}

void Transition::init() {
	texture = new Texture(path, renderer);
	black = new Texture("assets/black.png", renderer);
	startTime = SDL_GetTicks();
}

void Transition::cleanUp() {
	
}

void Transition::update(Uint32 delta) {
	timer = SDL_GetTicks() - startTime;
	if (timer > duration) stateManager->changeState(next);
	// check fade logic
	if (timer < fadeIn) {
		float percentage = 1.f - static_cast<float>(timer) / static_cast<float>(fadeIn);
		black->setAlpha(static_cast<int>(percentage * 255));
		// also fade in music but not out
		Audio::setVolume(0, SDL_MIX_MAXVOLUME * (1.f - percentage));
	}
	else if (timer > (duration - fadeOut)) {
		float percentage = 1.f - static_cast<float>(duration - timer) / static_cast<float>(fadeOut);
		black->setAlpha(static_cast<int>(percentage * 255));
	}
	else {
		black->setAlpha(0);
	}
}

void Transition::render(SDL_Renderer * renderer) {
	texture->render(renderer, x, y);
	black->render(renderer, true);
	State::render(renderer);
}