#include "menu.h"

#include "game.h"

Menu::Menu() {
	currentItem = 0;
	keyUpTimer = 0;
	keyDownTimer = 0;
	fullScreen = false;
}

Menu::~Menu() {
	// clean up textures
	for (Texture * t : menuItems) {
		delete t;
	}
	for (Texture * t : controlText) {
		delete t;
	}
	for (Texture * t : credits) {
		delete t;
	}
	delete black;
}

void Menu::init() {

	stateManager->createFont("menuTitle", "assets/fonts/munro/Munro.ttf", 84, Colour(255, 255, 255));
	title = new Texture(stateManager->getTextTexture("NULL INJECTION", "menuTitle"));

	stateManager->createFont("menuItem", "assets/fonts/munro/Munro.ttf", 64, Colour(240, 240, 255));
	Texture * texture1 = new Texture(stateManager->getTextTexture("PLAY", "menuItem"));
	menuItems.push_back(texture1);
	Texture * texture2 = new Texture(stateManager->getTextTexture("CONTROLS", "menuItem"));
	menuItems.push_back(texture2);
	Texture * texture3 = new Texture(stateManager->getTextTexture("WINDOWED", "menuItem"));
	menuItems.push_back(texture3);
	Texture * texture4 = new Texture(stateManager->getTextTexture("CREDITS", "menuItem"));
	menuItems.push_back(texture4);
	Texture * texture5 = new Texture(stateManager->getTextTexture("QUIT", "menuItem"));
	menuItems.push_back(texture5);
	updateMenuItems();
	black = new Texture("assets/black.png", renderer);
	fadeOut = false;

	stateManager->createFont("controlText", "assets/fonts/munro/Munro.ttf", 32, Colour(244, 244, 33));
	Texture * controlText1 = new Texture(stateManager->getTextTexture("Z: attack", "controlText"));
	controlText.push_back(controlText1);
	Texture * controlText2 = new Texture(stateManager->getTextTexture("X: roll", "controlText"));
	controlText.push_back(controlText2);
	Texture * controlText3 = new Texture(stateManager->getTextTexture("SPACE: jump", "controlText"));
	controlText.push_back(controlText3);
	Texture * controlText4 = new Texture(stateManager->getTextTexture("ARROWS: move", "controlText"));
	controlText.push_back(controlText4);

	stateManager->createFont("creditText", "assets/fonts/munro/Munro.ttf", 24, Colour(244, 33, 244));
	stateManager->createFont("creditTextContributor", "assets/fonts/munro/Munro.ttf", 32, Colour(33, 244, 244));
	Texture * creditText1 = new Texture(stateManager->getTextTexture("Programming:", "creditText"));
	credits.push_back(creditText1);
	Texture * creditText2 = new Texture(stateManager->getTextTexture("   Ian Wang", "creditTextContributor"));
	credits.push_back(creditText2);
	Texture * creditText3 = new Texture(stateManager->getTextTexture("Art:", "creditText"));
	credits.push_back(creditText3);
	Texture * creditText4 = new Texture(stateManager->getTextTexture("   Ian Wang", "creditTextContributor"));
	credits.push_back(creditText4);
	Texture * creditText5 = new Texture(stateManager->getTextTexture("Music:", "creditText"));
	credits.push_back(creditText5);
	Texture * creditText6 = new Texture(stateManager->getTextTexture("   Ian Wang", "creditTextContributor"));
	credits.push_back(creditText6);

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

	// set hint text
	stateManager->createFont("hint", "assets/fonts/munro/Munro.ttf", 16, Colour(155, 155, 155));
	hintText = new Texture(stateManager->getTextTexture("Use arrow keys and z to select", "hint"));
}

void Menu::cleanUp() {

}

void Menu::update(Uint32 delta) {
	background->update(static_cast<float>(delta));
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
			if (selectTimer == 0) select();
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
		if (selectTimer > 0) {
			selectTimer -= delta;
			if (selectTimer < 0) selectTimer = 0;
		}
	}
	else {
		int fadeOutTimer = SDL_GetTicks() - fadeOutStart;
		float alphaPercent = static_cast<float>(fadeOutTimer) / static_cast<float>(MENU_FADE_OUT_TIME);
		black->setAlpha(static_cast<Uint32>(alphaPercent * 255.f));
		if (fadeOutTimer >= MENU_FADE_OUT_TIME) {
			stateManager->changeState(new Game(false, -1, -1));
		}
	}
	// flash the hint text
	bool showHint = (SDL_GetTicks() / HINT_INTERVAL) % 2 == 0;
	if (showHint) hintText->setAlpha(255);
	else hintText->setAlpha(0);
}

void Menu::render(SDL_Renderer * renderer) {
	background->render(renderer, 0, 0);
	State::render(renderer);
	for (unsigned int i = 0; i < menuItems.size(); ++i) {
		menuItems.at(i)->render(renderer, 20, 64 * i + 180);
	}
	if (renderControl) renderControlText();
	if (renderCredit) renderCredits();
	// render the title
	title->render(renderer, MENU_TITLE_X, MENU_TITLE_Y);
	hintText->render(renderer, HINT_X, HINT_Y);
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
		renderControl = true;
		renderCredit = false;
	} break;
	case 2: {
		fullScreen = !fullScreen;
		SDL_DestroyWindow(stateManager->window);
		stateManager->window = SDL_CreateWindow("Fusion Blitz", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, fullScreen ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_BORDERLESS);
// 		SDL_SetWindowFullscreen(stateManager->window, fullScreen ? SDL_WINDOW_FULLSCREEN : 0);
		SDL_DestroyRenderer(renderer);
		stateManager->renderer = SDL_CreateRenderer(stateManager->window, -1, SDL_RENDERER_ACCELERATED);
		renderer = stateManager->renderer;
		if (!renderer) {
			ERR("Could not create new renderer");
			LOG(SDL_GetError());
		}
		// clear old texture vectors before resetting
		menuItems.clear();
		controlText.clear();
		credits.clear();
		init();
	} break;
	case 3: {
		renderCredit = true;
		renderControl = false;
	} break;
	case 4: {
		stateManager->exit();
	} break;
	default: {
		ERR("Menu item selection out of range.");
	}
	}
	selectTimer = KEY_TIMER;
}

void Menu::startGame() {
	if (!fadeOut) {
		fadeOutStart = SDL_GetTicks();
		black->setAlpha(255);
		fadeOut = true;
	}
}

void Menu::renderControlText() {
	for (unsigned int i = 0; i < controlText.size(); ++i) {
		controlText.at(i)->render(renderer, 290, i * 40 + 220);
	}
}

void Menu::renderCredits() {
	for (unsigned int i = 0; i < credits.size(); ++i) {
		credits.at(i)->render(renderer, 290, i * 40 + 220);
	}
}