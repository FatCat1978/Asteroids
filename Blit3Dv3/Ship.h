#pragma once
#include<Blit3D.h>

class Ship
{
public:
	Sprite *shotSprite;
	std::vector<Sprite *> spriteList;
	glm::vec2 velocity, position;
	float angle = 0;
	float shotTimer = 0;
	float radius = 27.f;
	int lives = 5;
	int frameNumber = 0;
	float thrustTimer = 0;
	bool thrusting = false;
	bool turningLeft = false;
	bool turningRight = false;

	float shotDelay = 0;
	bool isFiring = false;

	int score = 0;


	void Draw();
	void Update(float seconds);
	bool Shoot();

};