#include "background.h"

Background::Background(SDL_Renderer * renderer) {

}

Background::~Background() {

	for (element * e : layer1) {
		delete e;
	}
	for (element * e : layer2) {
		delete e;
	}
	for (element * e : layer3) {
		delete e;
	}

	delete largeGreen0;
	delete largeGreen1;
	delete smallGreen0;
	delete smallGreen1;
	delete tinyGreen0;
	delete tinyGreen1;

}

void Background::populate() {
	for (int i = 0; i < 200; ++i) {
		spawnBitsCloneWithMapYPositions();
	}
}

void Background::update(float delta) {
	for (element * b : layer1) {
		// single bit
		if (b->type == 0) {
			singleBit * ref = static_cast<singleBit*>(b);
			if (ref->up) ref->posY -= ref->speed * delta / 1000;
			else ref->posY += ref->speed * delta / 1000;
		}
	}
	for (element * b : layer2) {
		// single bit
		if (b->type == 0) {
			singleBit * ref = static_cast<singleBit*>(b);
			if (ref->up) ref->posY -= ref->speed * delta / 1000;
			else ref->posY += ref->speed * delta / 1000;
		}
	}
	for (element * b : layer3) {
		// single bit
		if (b->type == 0) {
			singleBit * ref = static_cast<singleBit*>(b);
			if (ref->up) ref->posY -= ref->speed * delta / 1000;
			else ref->posY += ref->speed * delta / 1000;
		}
	}
	// spawn some bits
	spawnBits();
	// remove out of range layer1
	for (int i = layer1.size() - 1; i >= 0; --i) {
		if (layer1.at(i)->posY < -1500|| layer1.at(i)->posY > 3000 || layer1.at(i)->posX < -1500 || layer1.at(i)->posX > 3000) {
			element * temp = layer1.at(i);
			layer1.erase(layer1.begin() + i);
			delete temp;
		}
	}
	for (int i = layer2.size() - 1; i >= 0; --i) {
		if (layer2.at(i)->posY < -1500 || layer2.at(i)->posY > 3000 || layer2.at(i)->posX < -1500 || layer2.at(i)->posX > 3000) {
			element * temp = layer2.at(i);
			layer2.erase(layer2.begin() + i);
			delete temp;
		}
	}
	for (int i = layer3.size() - 1; i >= 0; --i) {
		if (layer3.at(i)->posY < -1500 || layer3.at(i)->posY > 3000 || layer3.at(i)->posX < -1500 || layer3.at(i)->posX > 3000) {
			element * temp = layer3.at(i);
			layer3.erase(layer3.begin() + i);
			delete temp;
		}
	}
}

void Background::render(SDL_Renderer * renderer, int camX, int camY) {
	// layer 1 is closest bits
	for (element * b : layer1) {
		// single bit
		if (b->type == 0) {
			singleBit bit = *(static_cast<singleBit*>(b));
			bit.texture->render(renderer, bit.posX - camX, bit.posY - camY);
		}
	}
	// layer 2 is farther
	for (element * b : layer2) {
		// single bit
		if (b->type == 0) {
			singleBit bit = *(static_cast<singleBit*>(b));
			bit.texture->render(renderer, bit.posX - (camX / 2), bit.posY - (camY / 2));
		}
	}
	// layer 3 is REALLY far
	for (element * b : layer3) {
		// single bit
		if (b->type == 0) {
			singleBit bit = *(static_cast<singleBit*>(b));
			bit.texture->render(renderer, bit.posX - (camX / 4), bit.posY - (camY / 4));
		}
	}
}

void Background::spawnBits() {
	int key = randomNumber(6);

	switch (key) {
	// spawn a single bit somewhere
	case 0: {
		// randomly decide on a bit
		int bitSelect = randomNumber(2);
		int bitX = randomNumber(4000) - 1000;
		bool up = randomNumber(2) > 0 ? true : false;
		singleBit * bit = new singleBit(bitX, up ? 2999 : -1999);
		bit->texture = bitSelect > 0 ? largeGreen0 : largeGreen1;
		bit->up = up;
		bit->speed = BIT_BASE_SPEED_TOP + randomNumber(BIT_BASE_SPEED_TOP);
		layer1.emplace_back(bit);
	} break;

	// spawn a train of bits on layer 1
	case 1: {
		int numBits = randomNumber(30);
		bool up = randomNumber(2) > 0 ? true : false;
		int speed = BIT_BASE_SPEED_TOP + randomNumber(BIT_BASE_SPEED_TOP);
		int bitX = randomNumber(4000) - 1000;
		for (int i = 0; i < numBits; i++) {
			// randomly decide on a bit
			int bitSelect = randomNumber(2);
			singleBit * bit = new singleBit(bitX, up ? 2999 - 32 * i : -1499 + 32 * i);
			bit->texture = bitSelect > 0 ? largeGreen0 : largeGreen1;
			bit->up = up;
			bit->speed = speed;
			layer1.emplace_back(bit);
		}
	} break;
		// spawn a single bit somewhere
	case 2: {
		// randomly decide on a bit
		int bitSelect = randomNumber(2);
		int bitX = randomNumber(4000) - 1000;
		bool up = randomNumber(2) > 0 ? true : false;
		singleBit * bit = new singleBit(bitX, up ? 2999 : -1499);
		bit->texture = bitSelect > 0 ? smallGreen0 : smallGreen1;
		bit->up = up;
		bit->speed = BIT_BASE_SPEED_MID + randomNumber(BIT_BASE_SPEED_MID);
		layer2.emplace_back(bit);
	} break;

		// spawn a train of bits on layer 1
	case 3: {
		int numBits = randomNumber(30);
		bool up = randomNumber(2) > 0 ? true : false;
		int speed = BIT_BASE_SPEED_MID + randomNumber(BIT_BASE_SPEED_MID);
		int bitX = randomNumber(4000) - 1000;
		for (int i = 0; i < numBits; i++) {
			// randomly decide on a bit
			int bitSelect = randomNumber(2);
			singleBit * bit = new singleBit(bitX, up ? 2999 - 24 * i : -1499 + 24 * i);
			bit->texture = bitSelect > 0 ? smallGreen0 : smallGreen1;
			bit->up = up;
			bit->speed = speed;
			layer2.emplace_back(bit);
		}
	} break;
	case 4: {
		// randomly decide on a bit
		int bitSelect = randomNumber(2);
		int bitX = randomNumber(4000) - 1000;
		bool up = randomNumber(2) > 0 ? true : false;
		singleBit * bit = new singleBit(bitX, up ? 2999 : -1499);
		bit->texture = bitSelect > 0 ? smallGreen0 : smallGreen1;
		bit->up = up;
		bit->speed = BIT_BASE_SPEED_FAR + randomNumber(BIT_BASE_SPEED_FAR);
		layer3.emplace_back(bit);
	} break;

		// spawn a train of bits on layer 1
	case 5: {
		int numBits = randomNumber(30);
		bool up = randomNumber(2) > 0 ? true : false;
		int speed = BIT_BASE_SPEED_FAR + randomNumber(BIT_BASE_SPEED_FAR);
		int bitX = randomNumber(4000) - 1000;
		for (int i = 0; i < numBits; i++) {
			// randomly decide on a bit
			int bitSelect = randomNumber(2);
			singleBit * bit = new singleBit(bitX, up ? 2999 - 24 * i : -1499 + 24 * i);
			bit->texture = bitSelect > 0 ? smallGreen0 : smallGreen1;
			bit->up = up;
			bit->speed = speed;
			layer3.emplace_back(bit);
		}
	} break;
	}
}


void Background::spawnBitsCloneWithMapYPositions() {
	int key = randomNumber(6);

	switch (key) {
		// spawn a single bit somewhere
	case 0: {
		// randomly decide on a bit
		int bitSelect = randomNumber(2);
		int bitX = randomNumber(4000) - 1000;
		int bitY = randomNumber(4000) - 1000;
		bool up = randomNumber(2) > 0 ? true : false;
		singleBit * bit = new singleBit(bitX, bitY);
		bit->texture = bitSelect > 0 ? largeGreen0 : largeGreen1;
		bit->up = up;
		bit->speed = BIT_BASE_SPEED_TOP + randomNumber(BIT_BASE_SPEED_TOP);
		layer1.emplace_back(bit);
	} break;

		// spawn a train of bits on layer 1
	case 1: {
		int numBits = randomNumber(30);
		bool up = randomNumber(2) > 0 ? true : false;
		int speed = BIT_BASE_SPEED_TOP + randomNumber(BIT_BASE_SPEED_TOP);
		int bitX = randomNumber(4000) - 1000;
		int bitY = randomNumber(4000) - 1000;
		for (int i = 0; i < numBits; i++) {
			// randomly decide on a bit
			int bitSelect = randomNumber(2);
			singleBit * bit = new singleBit(bitX, up ? bitY - 32 * i : bitY + 32 * i);
			bit->texture = bitSelect > 0 ? largeGreen0 : largeGreen1;
			bit->up = up;
			bit->speed = speed;
			layer1.emplace_back(bit);
		}
	} break;
		// spawn a single bit somewhere
	case 2: {
		// randomly decide on a bit
		int bitSelect = randomNumber(2);
		int bitX = randomNumber(4000) - 1000;
		int bitY = randomNumber(4000) - 1000;
		bool up = randomNumber(2) > 0 ? true : false;
		singleBit * bit = new singleBit(bitX, bitY);
		bit->texture = bitSelect > 0 ? smallGreen0 : smallGreen1;
		bit->up = up;
		bit->speed = BIT_BASE_SPEED_MID + randomNumber(BIT_BASE_SPEED_MID);
		layer2.emplace_back(bit);
	} break;

		// spawn a train of bits on layer 1
	case 3: {
		int numBits = randomNumber(30);
		bool up = randomNumber(2) > 0 ? true : false;
		int speed = BIT_BASE_SPEED_MID + randomNumber(BIT_BASE_SPEED_MID);
		int bitX = randomNumber(4000) - 1000;
		int bitY = randomNumber(4000) - 1000;
		for (int i = 0; i < numBits; i++) {
			// randomly decide on a bit
			int bitSelect = randomNumber(2);
			singleBit * bit = new singleBit(bitX, up ? bitY - 24 * i : bitY + 24 * i);
			bit->texture = bitSelect > 0 ? smallGreen0 : smallGreen1;
			bit->up = up;
			bit->speed = speed;
			layer2.emplace_back(bit);
		}
	} break;
	case 4: {
		// randomly decide on a bit
		int bitSelect = randomNumber(2);
		int bitX = randomNumber(4000) - 1000;
		int bitY = randomNumber(4000) - 1000;
		bool up = randomNumber(2) > 0 ? true : false;
		singleBit * bit = new singleBit(bitX, bitY);
		bit->texture = bitSelect > 0 ? smallGreen0 : smallGreen1;
		bit->up = up;
		bit->speed = BIT_BASE_SPEED_FAR + randomNumber(BIT_BASE_SPEED_FAR);
		layer3.emplace_back(bit);
	} break;

		// spawn a train of bits on layer 1
	case 5: {
		int numBits = randomNumber(30);
		bool up = randomNumber(2) > 0 ? true : false;
		int speed = BIT_BASE_SPEED_FAR + randomNumber(BIT_BASE_SPEED_FAR);
		int bitX = randomNumber(4000) - 1000;
		int bitY = randomNumber(4000) - 1000;
		for (int i = 0; i < numBits; i++) {
			// randomly decide on a bit
			int bitSelect = randomNumber(2);
			singleBit * bit = new singleBit(bitX, up ? bitY - 14 * i : bitY + 14 * i);
			bit->texture = bitSelect > 0 ? smallGreen0 : smallGreen1;
			bit->up = up;
			bit->speed = speed;
			layer3.emplace_back(bit);
		}
	} break;
	}
}