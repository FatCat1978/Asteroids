#pragma once
#include<Blit3D.h>
#include "Ship.h"
class Asteroid{
public:
	int size = 3; //large by default
	int score = 0; //how much it's worth when picked up by player.
	float radius = 0.f;
	float angle = 0.f;
	glm::vec2 velocity, position;
	Sprite* AsteroidSprite;

	bool queue_delete = false; //if true, deletes next possible update cycle.

	void on_contact_ship();
	void collision(glm::vec2 velocity);

	void on_despawn();
	void on_spawn();
	void Draw();

	bool Update(float seconds); //returns true if deletion needed


	Asteroid(int asteroidsize, glm::vec2 initial_velocity);
};