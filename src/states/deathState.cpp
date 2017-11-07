#include "deathState.h"
#include "../audio/audio.h"
#include "menu.h"

DeathMenu::DeathMenu() {

}

DeathMenu::~DeathMenu() {
	delete background;
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
	Texture * texture2 = new Texture(stateManager->getTextTexture("QUIT", "menuItem"));
	menuItems.push_back(texture2);
	updateMenuItems();
	black = new Texture("assets/black.png", renderer);
	black->setAlpha(0);
	fadeOut = false;

	stateManager->createFont("gameOver", "assets/fonts/munro/Munro.ttf", 96, Colour(255, 0, 0));
	gameOverText = new Texture(stateManager->getTextTexture("GAME OVER...", "gameOver"));
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

	// start playing the music again
	Audio::setVolume(0, SDL_MIX_MAXVOLUME);
	Audio::playTrack("assets/music/menu.wav", 0, true);
}

void DeathMenu::cleanUp() {

}

void DeathMenu::update(Uint32 delta) {
	background->update(delta);
	if (!fadeOut) {
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
		if (keyPressed(SDL_SCANCODE_SPACE) || keyPressed(SDL_SCANCODE_RETURN) || keyPressed(SDL_SCANCODE_Z)) {
			select();
		}
		// update timers
		if (keyUpTimer > 0) {
			keyUpTimer -= delta;
			if (keyUpTimer < 0) keyUpTimer = 0;
		}
		if (keyDownTimer > 0) {
			keyDownTimer -= delta;
			if (keyDownTimer < 0) keyDownTimer = 0;
		}
	}
	else {
		int fadeOutTimer = SDL_GetTicks() - fadeOutStart;
		float alphaPercent = static_cast<float>(fadeOutTimer) / static_cast<float>(DEATH_FADE_OUT_TIME);
		black->setAlpha(static_cast<Uint32>(alphaPercent * 255.f));
		if (fadeOutTimer >= DEATH_FADE_OUT_TIME) {
			stateManager->changeState(new Menu());
		}
	}
}

void DeathMenu::render(SDL_Renderer * renderer) {
	background->render(renderer, 0, 0);
	State::render(renderer);
	for (unsigned int i = 0; i < menuItems.size(); ++i) {
		menuItems.at(i)->render(renderer, 20, 64 * i + 230);
	}
	gameOverText->render(renderer, GAME_OVER_X, GAME_OVER_Y);
	if (fadeOut) black->render(renderer, true);
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
		fadeOut = true;
		fadeOutStart = SDL_GetTicks();
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