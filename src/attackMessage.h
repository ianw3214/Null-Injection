#pragma once

#include <vector>

#include "QcEngine.h"

class Player;
class Enemy;

enum Target {
	PLAYER = 0,
	ENEMY = 1
};

struct AttackMessage {
	Target target;
	int damage;
	Shape * collisionBox;
	// contain a source position
	bool recoilRight;
	AttackMessage() : target(ENEMY), damage(1), collisionBox(new Rectangle()) {}
	AttackMessage(Target t, int d, Shape* s) : target(t), damage(d), collisionBox(s) {}
};

struct LinkedListQueue {
	LinkedListQueue * next;
	AttackMessage message;
};

class AttackMessager {

public:
	AttackMessager(Player * inputPlayer, std::vector<Enemy*>* inputEnemies);
	~AttackMessager();

	void addMessage(AttackMessage);
	bool hasMessage() const;
	AttackMessage dequeue();

private:

	LinkedListQueue * head;
	Player * player;
	std::vector<Enemy*>* enemies;

};