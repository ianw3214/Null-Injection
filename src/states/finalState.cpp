#include "finalState.h"

#include "menu.h"

#include "../audio/audio.h"

FinalState::FinalState(int finalTime, int health) {
	perfectGame = health == 5;
	time = finalTime;
}

FinalState::~FinalState() {
	// clean up textures
	for (Texture * t : menuItems) {
		delete t;
	}
	delete black;
	delete yourTime;
	delete timeTexture;
	delete background;
}

void FinalState::init() {
	currentItem = 0;
	keyUpTimer = 0;
	keyDownTimer = 0;
	stateManager->createFont("menuItem", "assets/fonts/munro/Munro.ttf", 64, Colour(200, 200, 200));
	Texture * texture1 = new Texture(stateManager->getTextTexture("MENU", "menuItem"));
	texture1->setAlpha(0);
	menuItems.push_back(texture1);
	Texture * texture2 = new Texture(stateManager->getTextTexture("QUIT", "menuItem"));
	texture2->setAlpha(0);
	menuItems.push_back(texture2);
	updateMenuItems();
	black = new Texture("assets/black.png", renderer);
	fadeOutTimer = 0;
	fadeOut = false;
	// create a font for the final timer
	stateManager->createFont("finalTimer", "assets/fonts/munro/Munro.ttf", 116, Colour(255, 255, 255));
	// calculate a time texture
	std::string time_rep;
	// finding values
	int seconds_rep = time / 1000;
	int minutes = seconds_rep / 60;
	int minutes_tens = minutes / 10;
	int minutes_ones = minutes % 10;
	int seconds = seconds_rep % 60;
	int seconds_ones = seconds % 10;
	int seconds_tens = seconds / 10;
	int ms_tens = (time / 100) % 10;
	int ms_ones = (time / 10) % 10;
	// put the values into the string
	time_rep += static_cast<char>(48 + minutes_tens);
	time_rep += static_cast<char>(48 + minutes_ones);
	time_rep += ':';
	time_rep += static_cast<char>(48 + seconds_tens);
	time_rep += static_cast<char>(48 + seconds_ones);
	time_rep += ':';
	time_rep += static_cast<char>(48 + ms_tens);
	time_rep += static_cast<char>(48 + ms_ones);
	timeTexture = new Texture(stateManager->getTextTexture(time_rep, "finalTimer"));
	timeTexture->setAlpha(0);
	
	stateManager->createFont("yourTime", "assets/fonts/munro/Munro.ttf", 64, Colour(180, 180, 200));
	yourTime = new Texture(stateManager->getTextTexture("Your Time: ", "yourTime"));
	yourTime->setAlpha(0);

	// set background fonts
	background = new Background(renderer);
	stateManager->createFont("bg0", "assets/fonts/munro/Munro.ttf", 32, Colour(0, 105, 0));
	background->largeGreen0 = new Texture(stateManager->getTextTexture("0", "bg0"));
	background->largeGreen1 = new Texture(stateManager->getTextTexture("1", "bg0"));
	stateManager->createFont("bg1", "assets/fonts/munro/Munro.ttf", 24, Colour(0, 76, 0));
	background->smallGreen0 = new Texture(stateManager->getTextTexture("0", "bg1"));
	background->smallGreen1 = new Texture(stateManager->getTextTexture("1", "bg1"));
	stateManager->createFont("bg2", "assets/fonts/munro/Munro.ttf", 14, Colour(0, 55, 0));
	background->tinyGreen0 = new Texture(stateManager->getTextTexture("0", "bg2"));
	background->tinyGreen1 = new Texture(stateManager->getTextTexture("1", "bg2"));
	background->populate();

	// create a font for perfect text
	stateManager->createFont("perfectText", "assets/fonts/munro/Munro.ttf", 24, Colour(255, 255, 0));
	perfectGameText = new Texture(stateManager->getTextTexture("Congratulations! You played a perfect Game!", "perfectText"));
	perfectGameText->setAlpha(0);

	// play the menu music
	Audio::playTrack("assets/music/menu.wav", 0, true);
	Audio::setVolume(0, SDL_MIX_MAXVOLUME);

	// cam shake variables
	xOffset = 0, yOffset = 0;
	shakeTimer = 0;

	titlePlayed = false;
	timePlayed = false;
	itemPlayed = false;
	loadin = true;
	startTime = SDL_GetTicks();

}

void FinalState::cleanUp() {
	
}

void FinalState::update(Uint32 delta) {
	background->update(delta);
	State::update(delta);
	if (!fadeOut) {
		if (shakeTimer >= 0) {
			xOffset = randomNumber(FINAL_SHAKE_INTENSITY) - FINAL_SHAKE_INTENSITY / 2;
			yOffset = randomNumber(FINAL_SHAKE_INTENSITY) - FINAL_SHAKE_INTENSITY / 2;
			shakeTimer -= delta;
		}
		else {
			xOffset = 0;
			yOffset = 0;
		}
		// if we are loading in, handle that logic fist
		if (loadin) {
			int timeDiff = SDL_GetTicks() - startTime;
			if (timeDiff > TEXT_1_TIME && !titlePlayed) {
				Audio::playTrack("assets/sfx/block.wav", 1, false);
				titlePlayed = true;
				yourTime->setAlpha(255);
				shakeTimer = FINAL_SHAKE_INTERVAL;
			}
			if (timeDiff > TEXT_2_TIME && !timePlayed) {
				Audio::playTrack("assets/sfx/block.wav", 1, false);
				timePlayed = true;
				timeTexture->setAlpha(255);
				shakeTimer = FINAL_SHAKE_INTERVAL;
			}
			// this is the last item loaded in
			if (timeDiff > TEXT_3_TIME && !itemPlayed) {
				Audio::playTrack("assets/sfx/block.wav", 1, false);
				itemPlayed = true;
				loadin = true;
				for (Texture * t : menuItems) {
					t->setAlpha(255);
				}
				shakeTimer = FINAL_SHAKE_INTERVAL;
			}
			if (timeDiff > TEXT_4_TIME) {
				loadin = false;
				if (perfectGame) perfectGameText->setAlpha(255);
			}
		}
		// otherwise, handle normal logic
		else {
			if (keyPressed(SDL_SCANCODE_UP) && keyUpTimer == 0) {
				currentItem = currentItem == 0 ? currentItem : currentItem - 1;
				keyUpTimer = KEY_TIMER;
				updateMenuItems();
				// play the menu blip sound
				Audio::playTrack("assets/sfx/menuBlip.wav", 1, false);
			}
			if (keyPressed(SDL_SCANCODE_DOWN) && keyDownTimer == 0) {
				currentItem = currentItem == menuItems.size() - 1 ? currentItem : currentItem + 1;
				keyDownTimer = KEY_TIMER;
				updateMenuItems();
				// play the menu blip sound
				Audio::playTrack("assets/sfx/menuBlip.wav", 1, false);
			}
			if (keyPressed(SDL_SCANCODE_Z) || keyPressed(SDL_SCANCODE_SPACE) || keyPressed(SDL_SCANCODE_RETURN)) {
				select();
			}
			// update timers
			if (keyUpTimer > 0) {
				keyUpTimer -= delta;
				if (keyUpTimer < 0) keyUpTimer = 0;
			}
			if (keyDownTimer> 0) {
				keyDownTimer -= delta;
				if (keyDownTimer < 0) keyDownTimer = 0;
			}
		}
	}
	else {
		fadeOutTimer -= delta;
		float alphaPercent = 1.f - static_cast<float>(fadeOutTimer) / static_cast<float>(FINAL_FADE_OUT_TIME);
		black->setAlpha(static_cast<Uint32>(alphaPercent * 255.f));
		if (fadeOutTimer <= 0) {
			stateManager->changeState(new Menu());
		}
	}
}

void FinalState::render(SDL_Renderer * renderer) {
	background->render(renderer, 0, 0);
	State::render(renderer);
	for (unsigned int i = 0; i < menuItems.size(); ++i) {
		menuItems.at(i)->render(renderer, ITEM_X_POS + xOffset, 64 * i + 230 + yOffset);
	}
	timeTexture->render(renderer, TIMER_X + xOffset, TIMER_Y + yOffset);
	yourTime->render(renderer, YOUR_TIME_X + xOffset, YOUR_TIME_Y + yOffset);
	perfectGameText->render(renderer, PERFECT_GAME_X, PERFECT_GAME_Y);
	if (fadeOut) black->render(renderer, true);
}

void FinalState::updateMenuItems() {
	for (unsigned int i = 0; i < menuItems.size(); ++i) {
		if (i == currentItem) {
			menuItems.at(i)->setColourModulation(Colour(255, 0, 0));
		}
		else {
			menuItems.at(i)->setColourModulation(Colour(200, 200, 200));
		}
	}
}

void FinalState::select() {
	// play the big menu blip sound
	Audio::playTrack("assets/sfx/bigMenuBlip.wav", 1, false);
	switch (currentItem) {
	case 0: {
		startGame();
	} break;
	case 1: {
		stateManager->exit();
	} break;
	default: {
		ERR("Menu item selection out of range.");
	}
	}
}

void FinalState::startGame() {
	fadeOutTimer = FINAL_FADE_OUT_TIME;
	black->setAlpha(255);
	fadeOut = true;
}