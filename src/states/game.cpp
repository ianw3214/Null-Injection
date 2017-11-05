#include "game.h"

Game::Game() {

}

Game::~Game() {
	delete middleMap;
	delete attackMessager;
}

void Game::init() {
	// hide the cursor
	// showCursor(false);

	// initialzie the messager
	attackMessager = new AttackMessager(player, &enemies);

	// instantiate a new player
	player = new Player(renderer, &mapCollisions, 500, 500, attackMessager);
	entities.push_back(player);

	// setup the map colliisons
	mapCollisions.emplace_back(new Rectangle(0, 1024, 1024, 50));
	mapCollisions.emplace_back(new Rectangle(0, 768, 448, 128));
	mapCollisions.emplace_back(new Rectangle(576, 704, 448, 64));
	mapCollisions.emplace_back(new Rectangle(768, 576, 256, 128));
	mapCollisions.emplace_back(new Rectangle(256, 384, 448, 128));
	mapCollisions.emplace_back(new Rectangle(0, 192, 192, 128));

	// setup the camera
	camX = 0, camY = 0;

	// setup the map
	middleMap = new Map("assets/map.png", renderer);

	// add an enemy
	for (int i = 0; i < 5; i++) {
		Enemy * enemy = new Enemy(randomNumber(1000), 0, &mapCollisions, renderer, attackMessager, player);
		entities.push_back(enemy);
		enemies.push_back(enemy);
	}

	// initialize static textures
	healthTexture = std::make_unique<Texture>("assets/heart.png", renderer);
}

void Game::cleanUp() {
	
}

void Game::update(Uint32 delta) {
	State::update(delta);
	// handle player movement
	if (keyPressed(SDL_SCANCODE_LEFT)) {
		player->move(1);
	}
	if (keyPressed(SDL_SCANCODE_RIGHT)) {
		player->move(0);
	}
	if (keyPressed(SDL_SCANCODE_SPACE)) {
		player->jump();
	}
	if (keyPressed(SDL_SCANCODE_X)) {
		player->roll();
	}
	if (keyPressed(SDL_SCANCODE_Z)) {
		player->attack();
	}
	// update camera position
	updateCameraPosition(delta);
	// update camera settings of each entity
	player->setCamX(camX);
	player->setCamY(camY);
	std::vector<Enemy*> removeEnemies;
	// update camera settings of each enemy and check if we should remove them
	for (Enemy * e : enemies) {
		e->setCamX(camX);
		e->setCamY(camY);
		if (e->REMOVE) {
			removeEnemies.push_back(e);
		}
	}
	// remove enemies accordingly
	for (Enemy * e : removeEnemies) {
		Enemy * temp = e;
		auto iter = std::find(enemies.begin(), enemies.end(), e);
		enemies.erase(iter);
		Entity * cast = dynamic_cast<Entity*>(e);
		auto iter2 = std::find(entities.begin(), entities.end(), cast);
		entities.erase(iter2);
		delete temp;
	}
	// update attacks and their effects to enemies
	while (attackMessager->hasMessage()) {
		processAndPopNextMessage();
	}
}

void Game::render(SDL_Renderer * renderer) {
	// render the map first
	middleMap->render(renderer, camX, camY);
	renderPlayerHealth();
	State::render(renderer);
}


void Game::processAndPopNextMessage() {
	if (!attackMessager->hasMessage()) {
		ERR("No more messages in queue");
		return;
	}
	// get the message and process it
	AttackMessage message = attackMessager->dequeue();
	if (message.target == PLAYER) {
		if (isColliding(*message.collisionBox, player->getCollisionBox()))
			player->takeDamage(message.damage);
	}
	if (message.target == ENEMY) {
		// damage the enemies
		for (Enemy * e : enemies) {
			if (isColliding(*(message.collisionBox), e->collisionBox))
				e->takeDamage(message.damage);
		}
	}
}

void Game::updateCameraPosition(Uint32 delta) {
	if (camX > player->getX() - CAM_MARGIN) {
		camX = player->getX() - CAM_MARGIN;
	}
	if (camX + SCREEN_WIDTH < player->getX() + 64 + CAM_MARGIN) {
		camX = player->getX() + 64 + CAM_MARGIN - SCREEN_WIDTH;
	}
	if (camY > player->getY() - CAM_MARGIN) {
		camY = player->getY() - CAM_MARGIN;
	}
	if (camY + SCREEN_HEIGHT < player->getY() + 64 + CAM_MARGIN) {
		camY = player->getY() + 64 + CAM_MARGIN - SCREEN_HEIGHT;
	}
}

void Game::renderPlayerHealth() {
	for (int i = 0; i < player->getHealth(); ++i) {
		healthTexture->render(renderer, i * 40 + HEALTH_MARGIN_X, HEALTH_MARGIN_Y);
	}
}