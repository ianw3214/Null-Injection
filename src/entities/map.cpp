#include "map.h"

Map::Map(std::string texturePath, SDL_Renderer* renderer) {
	this->texture = std::unique_ptr<Texture>(new Texture(texturePath, renderer));
}

Map::~Map() {

}

void Map::render(SDL_Renderer* renderer, int camX, int camY) {
	if (texture) texture->render(renderer, -camX, -camY);
}