#include "game.h"
#include "deathState.h"
#include "finalState.h"

Game::Game(bool resetMusic) {
	mapFile = "maps/tut.txt";
	if (resetMusic) {
		Audio::playTrack("assets/music/bgm.wav", 0, true);
		Audio::setVolume(0, SDL_MIX_MAXVOLUME);
	}
}

Game::Game(std::string path, bool resetMusic) {
	mapFile = path;
	if (resetMusic) {
		Audio::playTrack("assets/music/bgm.wav", 0, true);
		Audio::setVolume(0, SDL_MIX_MAXVOLUME);
	}
}

Game::~Game() {
	delete black;
	delete red;
	delete middleMap;
	delete bgMap;
	delete attackMessager;
	delete fadeOutBlack;
}

void Game::init() {
	// hide the cursor
	// showCursor(false);

	miniState = NORMAL;
	// initialzie the messager
	attackMessager = new AttackMessager(player, &enemies);

	// fade out things
	fadeOutBlack = new Texture("assets/black.png", renderer);
	fadeOut = false;
	fadeOutMusic = false;
	nextReset = false;
	nextFadeOut = false;

	// load the map
	loadFromFile(mapFile);
	bgMap = new Map("assets/map_bg.png", renderer);

	// initialize static textures
	healthTexture = std::make_unique<Texture>("assets/heart.png", renderer);

	if (miniState == INTRO) {
		black = new Texture("assets/black.png", renderer);
		introFadeTimes = { 0, 720, 1440, 3840, 4560, 5280, 7680, 8400, 9120, 11520, 12000, 12480, 12960, 13440, 13680, 13920, 14160, 14400, 14640, 14880, 15120, 15360 };
		currentFade = 0;
		introTimer = 0;
		introStart = SDL_GetTicks();
	}

	shakeTimer = 0;

	// death things
	stateManager->createFont("deathTitle", "assets/fonts/munro/Munro.ttf", 96, Colour(255, 0, 0));
	red = new Texture("assets/red.png", renderer);
	deathText = new Texture(stateManager->getTextTexture("You Died...", "deathTitle"));
	stateManager->createFont("deathText", "assets/fonts/munro/Munro.ttf", 44, Colour(205, 205, 205));
	nextStateText = new Texture(stateManager->getTextTexture("Press SPACE to continue...", "deathText"));

	// stage win things
	stateManager->createFont("stageBeatTitle", "assets/fonts/munro/Munro.ttf", 96, Colour(255, 255, 255));
	stageBeatText1 = new Texture(stateManager->getTextTexture("STAGE", "stageBeatTitle"));
	stageBeatText2 = new Texture(stateManager->getTextTexture("COMPLETE", "stageBeatTitle"));
	stageBeatText3 = new Texture(stateManager->getTextTexture("Press Z to proceed...", "deathText"));
	white = new Texture("assets/white.png", renderer);
	stageBeatText1->setAlpha(0);
	stageBeatText2->setAlpha(0);

	stateManager->createFont("timer", "assets/fonts/munro/Munro.ttf", 64, Colour(155, 155, 155));
	stateManager->createFont("timerSmall", "assets/fonts/munro/Munro.ttf", 32, Colour(155, 155, 155));
	timerStart = SDL_GetTicks();
	cTimer = 0;
	timerOn = false;

}

void Game::cleanUp() {
	// stop playing music
}

void Game::update(Uint32 delta) {
	// if we are fading out, just do that and don't do anything else
	if (fadeOut) {
		int timeDiff = SDL_GetTicks() - fadeOutStart;
		if (timeDiff > FADE_OUT_TIME) {
			if (miniState == DEATH) stateManager->changeState(new DeathMenu());
			if (miniState == COMPLETE) nextLevel();
		}
		else {
			float fadePercent = static_cast<float>(timeDiff) / static_cast<float>(FADE_OUT_TIME);
			fadeOutBlack->setAlpha(static_cast<int>(fadePercent * 255));
			// fade out music as well
			if (fadeOutMusic) {
				Audio::setVolume(0, SDL_MIX_MAXVOLUME * (1.f - fadePercent));
			}
		}
		return;
	}
	// update in slow motion if we are dead or beat the level
	if (miniState == DEATH || miniState == COMPLETE) {
		// update first few frames in slow motion
		if (numSlowMoFrames < NUM_SLOW_MO_FRAMES) {
			deathSlowMoFrameCounter--;
			if (deathSlowMoFrameCounter <= 0) {
				deathSlowMoFrameCounter = SLOW_MO_FRAMES;
				numSlowMoFrames++;
			}
			else {
				return;
			}
		}
	}
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
			if ((unsigned int)currentFade >= introFadeTimes.size()) {
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
		// update red overlay
		float alphaPercentage = 1.f - static_cast<float>(timeDiff) / static_cast<float>(RED_OVERLAY_PERSIST_AFTER);
		if (alphaPercentage < 0.5f) alphaPercentage = 0.5f;
		red->setAlpha(static_cast<int>(alphaPercentage * 255));
		// keep updating player animations
		player->updateAnimation(static_cast<float>(delta));
		// keep updating enemies
		for (Enemy * e : enemies) {
			e->updateSimple(static_cast<float>(delta));
		}
		// update camera shake timer with trailing intensity
		if (shakeTimer > 0) {
			float shakePercent = 1.f - static_cast<float>(timeDiff) / static_cast<float>(DEATH_SHAKE_TIME);
			if (shakePercent < .1f) shakePercent = .1f;
			int newIntensity = static_cast<int>(shakePercent * intensity);
			camOffsetX = randomNumber(newIntensity) - newIntensity / 2;
			camOffsetY = randomNumber(newIntensity) - newIntensity / 2;
			shakeTimer -= delta;
		}
		else {
			camOffsetX = 0;
			camOffsetY = 0;
		}
		// update title text alpha
		float titleAlpha = 1.f;
		if (timeDiff >= SHOW_TITLE_AT && timeDiff <= TITLE_FULL_AT)
			titleAlpha = static_cast<float>(timeDiff - SHOW_TITLE_AT) / static_cast<float>(TITLE_FULL_AT - SHOW_TITLE_AT);
		deathText->setAlpha(static_cast<int>(titleAlpha * 255));
		if (timeDiff > TITLE_FULL_AT) {
			if (keyPressed(SDL_SCANCODE_SPACE) || keyPressed(SDL_SCANCODE_RETURN) || keyPressed(SDL_SCANCODE_Z)) {
				fadeOut = true;
				fadeOutStart = SDL_GetTicks();
			}
		}
		// render the blinking text
		if (timeDiff > SHOW_DEATH_TEXT_AT) {
			bool on = static_cast<int>((timeDiff - SHOW_DEATH_TEXT_AT)/ DEATH_TEXT_BLINK_TIME) % 2 == 0;
			nextStateText->setAlpha(on ? 255 : 0);
		}
		return;
	}
	// play the stage complete state if set
	if (miniState == COMPLETE) {
		int timeDiff = SDL_GetTicks() - levelBeatTime;
		// show title after a certain amount of time
		if (timeDiff > SHOW_COMPLETE_TITLE_AT) {
			stageBeatText1->setAlpha(255);
			stageBeatText2->setAlpha(255);
		}
		else {
			stageBeatText1->setAlpha(0);
			stageBeatText2->setAlpha(0);
		}
		// update white overlay
		float alphaPercentage = 1.f - static_cast<float>(timeDiff) / static_cast<float>(WHITE_OVERLAY_TRAIL);
		if (alphaPercentage < 0.0f) alphaPercentage = 0.0f;
		white->setAlpha(static_cast<int>(alphaPercentage * 255));
		// render the blinking text
		if (timeDiff > SHOW_WIN_TEXT_AT) {
			bool on = static_cast<int>((timeDiff - SHOW_WIN_TEXT_AT) / WIN_TEXT_BLINK_TIME) % 2 == 0;
			stageBeatText3->setAlpha(on ? 255 : 0);
		}
		else {
			stageBeatText3->setAlpha(0);
		}
		// go the the next level if key pressed
		if (timeDiff > CAN_GOTO_NEXT_LEVEL) {
			if (keyPressed(SDL_SCANCODE_RETURN) || keyPressed(SDL_SCANCODE_Z)) {
				fadeOut = true;
				fadeOutStart = SDL_GetTicks();
			}
		}
		// don't return for a complete stage, just add more functionalities
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
	// don't update states if we are already complete
	if (miniState == COMPLETE) return;
	// check if player beat the level
	if (killedEnemies == numEnemies) {
		levelBeatTime = SDL_GetTicks();
		miniState = COMPLETE;
		numSlowMoFrames = 0;
		shakeTimer = WIN_SHAKE_TIME;
		intensity = WIN_SHAKE_INTENSITY;
	}
	// go to death menu if player died
	// however, don't die if we beat the level first
	if (player->getHealth() <= 0 && miniState != COMPLETE) {
		miniState = DEATH;
		deathStartTime = SDL_GetTicks();
		deathSlowMoFrameCounter = SLOW_MO_FRAMES;
		numSlowMoFrames = 0;
		// add screen shake
		shakeTimer = DEATH_SHAKE_TIME;
		intensity = DEATH_SHAKE_INTENSITY;
	}
	// update the timer
	if (miniState == NORMAL && !timerOn) timerOn = true;
	else timerOn = false;
	if (timerOn) cTimer = SDL_GetTicks() - timerStart;
}

void Game::render(SDL_Renderer * renderer) {
	// render the map first
	bgMap->render(renderer, (camX + camOffsetX) / 2, (camY + camOffsetY) / 2);
	middleMap->render(renderer, (camX + camOffsetX), (camY + camOffsetY));
	// if we are fading out, just render that
	if (fadeOut) {
		player->render(renderer);
		for (Enemy * e : enemies) {
			e->render(renderer);
		}
		fadeOutBlack->render(renderer, true);
		return;
	}
	// render specialized things only if playing intro
	if (miniState == INTRO) {
		player->render(renderer);
		for (Enemy * e : enemies) {
			e->render(renderer);
		}
		black->render(renderer, true);
		return;
	}
	// render specialized things only if playing death transition
	if (miniState == DEATH) {
		State::render(renderer);
		red->render(renderer, true);
		// render the title if a certain amount of time passed
		if ((SDL_GetTicks() - deathStartTime) > SHOW_TITLE_AT) {
			deathText->render(renderer, TITLE_X, TITLE_Y);
		}
		if (SDL_GetTicks() - deathStartTime > SHOW_DEATH_TEXT_AT) {
			nextStateText->render(renderer, DEATH_TEXT_X, DEATH_TEXT_Y);
		}
		return;
	}
	if (miniState == COMPLETE) {
		State::render(renderer);
		renderPlayerHealth();
		stageBeatText1->render(renderer, COMPLETE_TEXT_1_X, COMPLETE_TEXT_1_Y);
		stageBeatText2->render(renderer, COMPLETE_TEXT_2_X, COMPLETE_TEXT_2_Y);
		stageBeatText3->render(renderer, WIN_TEXT_X, WIN_TEXT_Y);
		white->render(renderer, true);
		return;
	}
	renderPlayerHealth();
	renderTimer();
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
			player->takeDamage(message.damage, message.recoilRight);
	}
	if (message.target == ENEMY) {
		// damage the enemies
		for (Enemy * e : enemies) {
			if (isColliding(*(message.collisionBox), e->collisionBox)) {
				if (e->takeDamage(message.damage, message.recoilRight)) {
					killedEnemies++;
				}
			}
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
	next = "";
	std::ifstream mapFile(path.c_str());
	if (mapFile.is_open()) {
		std::string line;
		while (getline(mapFile, line)) {
			std::vector<std::string> tokens = split(line, ' ');
			// comments
			if (line[0] == '#') continue;
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
				numEnemies++;
			}
			// set next level
			if (line[0] == 'N') {
				next = tokens[1];
			}
			if (tokens.size() == 3) {
				// check if we should play intro
				if (tokens[0] == "I" && tokens[1] == "I" && tokens[2] == "I") {
					miniState = INTRO;
				}
				// check if we should fade out music
				if (tokens[0] == "F" && tokens[1] == "F" && tokens[2] == "F") {
					fadeOutMusic = true;
				}
			}
		}
	}
	else {
		ERR("Failed to open map file: " << path);
		return;
	}
	// use a default map if the next map is not set
	if (next == "") {
		next = "FINISH";
	}
}

void Game::renderTimer() {
	// rounds down
	int seconds_rep = cTimer / 1000;
	int minutes = seconds_rep / 60;
	int minutes_tens = minutes / 10;
	int minutes_ones = minutes % 10;
	int seconds = seconds_rep % 60;
	int seconds_ones = seconds % 10;
	int seconds_tens = seconds / 10;
	std::string time_str = "";
	time_str += static_cast<char>(48 + minutes_tens);
	time_str += static_cast<char>(48 + minutes_ones);
	time_str += " : ";
	time_str += static_cast<char>(48 + seconds_tens);
	time_str += static_cast<char>(48 + seconds_ones);
	Texture text(stateManager->getTextTexture(time_str, "timer"));
	text.render(renderer, TIMER_TEXT_X, TIMER_TEXT_Y);
	// also render milliseconds
	int ms_tens = (cTimer / 100) % 10;
	int ms_ones = (cTimer / 10) % 10;
	std::string ms_str = "";
	ms_str += static_cast<char>(48 + ms_tens);
	ms_str += static_cast<char>(48 + ms_ones);
	Texture text2(stateManager->getTextTexture(ms_str, "timerSmall"));
	text2.render(renderer, TIMER_MS_TEXT_X, TIMER_MS_TEXT_Y);
}

void Game::nextLevel() {
	if (next == "FINISH") {
		stateManager->changeState(new FinalState(cTimer));
	}
	else {
		stateManager->changeState(new Game(next, true));
	}
}