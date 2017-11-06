#include "finalState.h"

#include "menu.h"

#include "../audio/audio.h"

FinalState::FinalState(int finalTime) {
	time = finalTime;
}

FinalState::~FinalState() {
	// clean up textures
	for (Texture * t : menuItems) {
		delete t;
	}
	delete black;
	delete yourTime;
}

void FinalState::init() {
	currentItem = 0;
	keyUpTimer = 0;
	keyDownTimer = 0;
	stateManager->createFont("menuItem", "assets/fonts/munro/Munro.ttf", 64, Colour(200, 200, 200));
	Texture * texture1 = new Texture(stateManager->getTextTexture("MENU", "menuItem"));
	menuItems.push_back(texture1);
	Texture * texture3 = new Texture(stateManager->getTextTexture("QUIT", "menuItem"));
	menuItems.push_back(texture3);
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
	// play the menu music
	Audio::playTrack("assets/music/menu.wav", 0, true);
	Audio::setVolume(0, SDL_MIX_MAXVOLUME);

	stateManager->createFont("yourTime", "assets/fonts/munro/Munro.ttf", 64, Colour(180, 180, 200));
	yourTime = new Texture(stateManager->getTextTexture("Your Time: ", "yourTime"));
}

void FinalState::cleanUp() {

}

void FinalState::update(Uint32 delta) {
	State::update(delta);
	if (!fadeOut) {
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
	else {
		fadeOutTimer -= delta;
		float alphaPercent = 1.f - static_cast<float>(fadeOutTimer) / static_cast<float>(FADE_OUT_TIME);
		black->setAlpha(static_cast<Uint32>(alphaPercent * 255.f));
		if (fadeOutTimer <= 0) {
			stateManager->changeState(new Menu());
		}
	}
}

void FinalState::render(SDL_Renderer * renderer) {
	State::render(renderer);
	for (unsigned int i = 0; i < menuItems.size(); ++i) {
		menuItems.at(i)->render(renderer, 20, 64 * i + 230);
	}
	timeTexture->render(renderer, TIMER_X, TIMER_Y);
	yourTime->render(renderer, YOUR_TIME_X, YOUR_TIME_Y);
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
	fadeOutTimer = FADE_OUT_TIME;
	black->setAlpha(255);
	fadeOut = true;
}