#pragma once

#include <string>

#include "QcEngine.h"

class Transition : public State {

public:

	Transition(State * next, Uint32 duration, std::string path, int x, int y, int fadeIn, int fadeOut);
	~Transition();

	void init();
	void cleanUp();

	void update(Uint32 delta);
	void render(SDL_Renderer*);

private:
	Texture * texture;
	std::string path;
	int x, y;
	State * next;

	int startTime;
	int timer;
	int duration;

	int fadeIn, fadeOut;
	Texture * black;
};