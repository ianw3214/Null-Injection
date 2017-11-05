#include "menu.h"

#include "game.h"

Menu::Menu() {

}

Menu::~Menu() {
	// clean up textures
	for (Texture * t : menuItems) {
		delete t;
	}
	delete black;
}

void Menu::init() {
	currentItem = 0;
	keyUpTimer = 0;
	keyDownTimer = 0;
	stateManager->createFont("menuItem", "assets/fonts/munro/Munro.ttf", 64, Colour(200, 200, 200));
	Texture * texture1 = new Texture(stateManager->getTextTexture("PLAY", "menuItem"));
	menuItems.push_back(texture1);
	Texture * texture2 = new Texture(stateManager->getTextTexture("TEMP", "menuItem"));
	menuItems.push_back(texture2);
	Texture * texture3 = new Texture(stateManager->getTextTexture("QUIT", "menuItem"));
	menuItems.push_back(texture3);
	updateMenuItems();
	black = new Texture("assets/black.png", renderer);
	fadeOutTimer = 0;
	fadeOut = false;
	Audio::mute();
}

void Menu::cleanUp() {

}

void Menu::update(Uint32 delta) {
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
		if (keyPressed(SDL_SCANCODE_SPACE) || keyPressed(SDL_SCANCODE_RETURN)) {
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
			stateManager->changeState(new Game());
		}
	}
}

void Menu::render(SDL_Renderer * renderer) {
	State::render(renderer);
	for (unsigned int i = 0; i < menuItems.size(); ++i) {
		menuItems.at(i)->render(renderer, 20, 64 * i + 230);
	}
	if (fadeOut) black->render(renderer, true);
	
}

void Menu::updateMenuItems() {
	for (unsigned int i = 0; i < menuItems.size(); ++i) {
		if (i == currentItem) {
			menuItems.at(i)->setColourModulation(Colour(255, 0, 0));
		}
		else {
			menuItems.at(i)->setColourModulation(Colour(200, 200, 200));
		}
	}
}

void Menu::select() {
	// play the big menu blip sound
	Audio::playTrack("assets/sfx/bigMenuBlip.wav", 1, false);
	switch (currentItem) {
	case 0: {
		startGame();
	} break;
	case 1: {
		LOG("LOL");
	} break;
	case 2: {
		stateManager->exit();
	} break;
	default: {
		ERR("Menu item selection out of range.");
	}
	}
}

void Menu::startGame() {
	fadeOutTimer = FADE_OUT_TIME;
	black->setAlpha(255);
	fadeOut = true;
}