#include "deathState.h"
#include "../audio/audio.h"
#include "menu.h"

DeathMenu::DeathMenu() {

}

DeathMenu::~DeathMenu() {
	// clean up textures
	for (Texture * t : menuItems) {
		delete t;
	}
}

void DeathMenu::init() {
	currentItem = 0;
	keyUpTimer = 0;
	keyDownTimer = 0;
	stateManager->createFont("menuItem", "assets/fonts/munro/Munro.ttf", 64, Colour(200, 200, 200));
	Texture * texture1 = new Texture(stateManager->getTextTexture("MENU", "menuItem"));
	menuItems.push_back(texture1);
	Texture * texture2 = new Texture(stateManager->getTextTexture("TEMP", "menuItem"));
	menuItems.push_back(texture2);
	Texture * texture3 = new Texture(stateManager->getTextTexture("QUIT", "menuItem"));
	menuItems.push_back(texture3);
	updateMenuItems();
}

void DeathMenu::cleanUp() {

}

void DeathMenu::update(Uint32 delta) {
	State::update(delta);
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

void DeathMenu::render(SDL_Renderer * renderer) {
	State::render(renderer);
	for (unsigned int i = 0; i < menuItems.size(); ++i) {
		menuItems.at(i)->render(renderer, 20, 64 * i + 230);
	}
}

void DeathMenu::updateMenuItems() {
	for (unsigned int i = 0; i < menuItems.size(); ++i) {
		if (i == currentItem) {
			menuItems.at(i)->setColourModulation(Colour(255, 0, 0));
		}
		else {
			menuItems.at(i)->setColourModulation(Colour(200, 200, 200));
		}
	}
}

void DeathMenu::select() {
	// play the big menu blip sound
	Audio::playTrack("assets/sfx/bigMenuBlip.wav", 1, false);
	switch (currentItem) {
	case 0: {
		stateManager->changeState(new Menu());
	} break;
	case 1: {
		LOG("LOL");
	} break;
	case 2: {
		stateManager->exit();
	} break;
	default: {
		ERR("DeathMenu item selection out of range.");
	}
	}
}