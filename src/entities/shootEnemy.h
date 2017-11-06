#pragma once

#include <vector>
#include <cmath>
#include <valarray>

#include "QcEngine.h"

#include "enemy.h"

#define SHOOT_ATTACK_TIME 300
#define PROJECTILE_SPEED 500

#define SHOOT_ATTACK_INTERVAL 1000

class ShootEnemy : public Enemy {

struct Projectile {
	Texture * texture;
	Circle collision;
	int x, y;
	double angle;
};

public:
	ShootEnemy(int x, int y, std::vector<Shape*>* inputMap, SDL_Renderer * renderer, AttackMessager * messager, Player * playerRef);
	~ShootEnemy();

	void attack(bool right);

	void render(SDL_Renderer * renderer);
	void renderProjectiles(SDL_Renderer* renderer, int camX, int camY);
	void update(float delta);
	
private:
	int shouldAttackTimer;

	// store projectiles fired
	std::vector<Projectile> projectiles;
	void updateProjectiles(float delta);
};