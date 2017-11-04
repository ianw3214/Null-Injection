#pragma once

#include <string>
#include <memory>

#include "QcEngine.h"

class Map {

public:
	Map(std::string texturePath, SDL_Renderer* renderer);
	~Map();

	void render(SDL_Renderer* renderer, int camX, int camY);

private:

	std::unique_ptr<Texture> texture;
};