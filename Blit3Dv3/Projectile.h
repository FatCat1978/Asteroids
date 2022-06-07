#pragma once
#include<Blit3D.h>
#include "Ship.h"

class Projectile
{
public:
	float radius = 0.f;
	float angle = 0.f;
	glm::vec2 velocity, position;

	float lifespan = 0.f; //lifespan in seconds.

	Sprite* shotSprite;

	bool queue_for_del = false;

	void on_contact_proj(); //called when the Projectile comes into contact with another projectile.

	void on_contact_ship(); //called when the projectile touches a ship.

	void on_contact_asteroid(); //you get the idea by now.

	void on_despawn();
	void on_spawn();


	void Draw();

	bool Update(float seconds); //returns true if deletion needed



	Projectile(Ship* owner, std::string media, int iconsize, float radius = 0.f, float slifespan = 0.f);
};