#include "game.h"
#include "deathState.h"

Game::Game() {
	mapFile = "maps/tut.txt";
	miniState = NORMAL;
}

Game::Game(std::string path) {
	mapFile = path;
}

Game::~Game() {
	delete middleMap;
	delete bgMap;
	delete attackMessager;
}

void Game::init() {
	// hide the cursor
	// showCursor(false);

	// initialzie the messager
	attackMessager = new AttackMessager(player, &enemies);

	/*
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
	mapCollisions.emplace_back(new Rectangle(0, 0, 64, 1128));
	mapCollisions.emplace_back(new Rectangle(960, 0, 64, 1128));
	mapCollisions.emplace_back(new Rectangle(0, -64, 1024, 64));

	// setup the camera
	camX = 0, camY = 0;

	// setup the map
	middleMap = new Map("assets/map.png", renderer);
	bgMap = new Map("assets/map_bg.png", renderer);

	// add an enemy
	for (int i = 0; i < 5; i++) {
		Enemy * enemy = new Enemy(randomNumber(1000), 0, &mapCollisions, renderer, attackMessager, player);
		entities.push_back(enemy);
		enemies.push_back(enemy);
	}
	*/
	loadFromFile(mapFile);
	bgMap = new Map("assets/map_bg.png", renderer);

	// initialize static textures
	healthTexture = std::make_unique<Texture>("assets/heart.png", renderer);

	// temporarily mute the game
	// Audio::mute();
	Audio::playTrack("assets/music/bgm.wav", 0, true);

	if (miniState == INTRO) {
		black = new Texture("assets/black.png", renderer);
		introFadeTimes = { 0, 720, 1440, 3840, 4560, 5280, 7680, 8400, 9120, 11520, 12000, 12480, 12960, 13440, 13680, 13920, 14160, 14400, 14640, 14880, 15120, 15360 };
		currentFade = 0;
		introTimer = 0;
		introStart = SDL_GetTicks();
	}

	shakeTimer = 0;

}

void Game::cleanUp() {
	// stop playing music
}

void Game::update(Uint32 delta) {
	// play the intro first if set
	if (miniState == INTRO) {
		// set the start time if it is the first run through
		if (introTimer >= introFadeTimes[currentFade] + INTRO_FADE_DELAY) {
			// update the camera positions
			camX = randomNumber(800), camY = randomNumber(800);
			player->setCamX(camX);
			player->setCamY(camY);
			for (Enemy * e : enemies) {
				e->setCamX(camX);
				e->setCamY(camY);
			}
			currentFadeTime = introFadeTimes[currentFade];
			currentFade++;
			// check if we are done the intro
			if (currentFade >= introFadeTimes.size()) {
				miniState = NORMAL;
			}
		}
		// do the fade
		float alphaPercent = static_cast<float>(introTimer - currentFadeTime) / static_cast<float>(INTRO_FADE_BAR);
		if (alphaPercent > 1.f) alphaPercent = 1.f;
		black->setAlpha(static_cast<Uint8>(alphaPercent * 255.f));
		introTimer = SDL_GetTicks() - introStart;
		return;
	}
	// play the death state first if set
	if (miniState == DEATH) {
		int timeDiff = SDL_GetTicks() - deathStartTime;
		if (keyPressed(SDL_SCANCODE_SPACE) || keyPressed(SDL_SCANCODE_RETURN) || keyPressed(SDL_SCANCODE_Z)) {
			SDL_Delay(200);
			stateManager->changeState(new DeathMenu());
		}
		// update first few frames in slow motion
		if (numSlowMoFrames < NUM_SLOW_MO_FRAMES) {
			deathSlowMoFrameCounter--;
			if (deathSlowMoFrameCounter <= 0) {
				deathSlowMoFrameCounter = SLOW_MO_FRAMES;
			}
			else {
				return;
			}
		}
		// keep updating player animations
		player->updateAnimation(delta);
		// keep updating enemies
		for (Enemy * e : enemies) {
			e->updateSimple(delta);
		}
		return;
	}
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
		if (player->attack()) {
			shakeTimer = PLAYER_ATTACK_SHAKE_TIME;
			intensity = PLAYER_ATTACK_SHAKE_INTENSITY;
		}
	}
	// update camera shake timer
	if (shakeTimer > 0) {
		camOffsetX = randomNumber(intensity) - intensity / 2;
		camOffsetY = randomNumber(intensity) - intensity / 2;
		shakeTimer -= delta;
	}
	else {
		camOffsetX = 0;
		camOffsetY = 0;
	}
	// update camera position
	updateCameraPosition(delta);
	// update camera settings of each entity
	player->setCamX(camX + camOffsetX);
	player->setCamY(camY + camOffsetY);
	std::vector<Enemy*> removeEnemies;
	// update camera settings of each enemy and check if we should remove them
	for (Enemy * e : enemies) {
		e->setCamX(camX + camOffsetX);
		e->setCamY(camY + camOffsetY);
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
	// go to death menu if player died
	// TODO: make transition smoother
	if (player->getHealth() <= 0) {
		miniState = DEATH;
		deathStartTime = SDL_GetTicks();
		deathSlowMoFrameCounter = SLOW_MO_FRAMES;
		numSlowMoFrames = 0;
	}
}

void Game::render(SDL_Renderer * renderer) {
	// render the map first
	bgMap->render(renderer, (camX + camOffsetX) / 2, (camY + camOffsetY) / 2);
	middleMap->render(renderer, (camX + camOffsetX), (camY + camOffsetY));
	// render specialized things only if playing intro
	if (miniState == INTRO) {
		player->render(renderer);
		for (Enemy * e : enemies) {
			e->render(renderer);
		}
		black->render(renderer, true);
		return;
	}
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

std::vector<std::string> Game::split(std::string str, char delimiter) {
	std::string token = "";
	std::vector<std::string> result;
	for (char c : str) {
		if (c == delimiter) {
			// only add the token if it's nonempty or if we don't want to remove empty tokens
			result.emplace_back(token);
			token = "";
		}
		else {
			token += c;
		}
	}
	result.emplace_back(token);
	return result;
}

void Game::loadFromFile(std::string path) {
	std::ifstream mapFile(path.c_str());
	if (mapFile.is_open()) {
		std::string line;
		while (getline(mapFile, line)) {
			std::vector<std::string> tokens = split(line, ' ');
			// background data
			if (line[0] == 'B' && line[1] == 'G') {
				middleMap = new Map(tokens[1], renderer);
			}
			if (line[0] == 'P') {
				player = new Player(renderer, &mapCollisions, std::stoi(tokens[1]), std::stoi(tokens[2]), attackMessager);
				entities.push_back(player);
			}
			// add collision shape
			if (line[0] == 'C') {
				mapCollisions.emplace_back(new Rectangle(std::stoi(tokens[1]), std::stoi(tokens[2]), std::stoi(tokens[3]), std::stoi(tokens[4])));
			}
			// add basic enemy
			if (line[0] == 'E') {
				Enemy * enemy = new Enemy(std::stoi(tokens[1]), std::stoi(tokens[2]), &mapCollisions, renderer, attackMessager, player);
				entities.push_back(enemy);
				enemies.push_back(enemy);
			}
		}
	}
	else {
		ERR("Failed to open map file: " << path);
		return;
	}
}