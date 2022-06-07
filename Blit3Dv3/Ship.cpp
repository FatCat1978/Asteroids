#include "Ship.h"
#include "Projectile.h"

extern Blit3D* blit3D;
extern std::vector<Projectile*> Projectiles;

void Ship::Draw()
{
	//change ship angle because my graphics face "up", not "right"
	spriteList[frameNumber]->angle = angle - 90;
	
	//draw main ship sprite
	spriteList[frameNumber]->Blit(position.x, position.y);

	//redraw if too close to an edge
	//left
	if(position.x < radius + 10.f) spriteList[frameNumber]->Blit(position.x + 1920.f, position.y);
	//right
	if (position.x > 1920.f - (radius + 10.f)) spriteList[frameNumber]->Blit(position.x - 1920.f, position.y);
	//down
	if (position.y < radius + 10.f) spriteList[frameNumber]->Blit(position.x, position.y + 1080.f);
	//up
	if (position.y > 1080.f - (radius + 10.f)) spriteList[frameNumber]->Blit(position.x, position.y - 1080.f);

	//copies for 4 diagonal corners
	spriteList[frameNumber]->Blit(position.x + 1920.f, position.y + 1080.f);
	spriteList[frameNumber]->Blit(position.x - 1920.f, position.y - 1080.f);
	spriteList[frameNumber]->Blit(position.x - 1920.f, position.y + 1080.f);
	spriteList[frameNumber]->Blit(position.x + 1920.f, position.y - 1080.f);
}

void Ship::Update(float seconds)
{
	//handle turning
	if (turningLeft)
	{
		angle += 180.f * seconds;
	}

	if (turningRight)
	{
		angle -= 180.f * seconds;
	}

	if (shotDelay > 0)
		shotDelay -= seconds;

	if (isFiring)
		if ((shotDelay <= 0))
		{
			shotDelay = 0.2; //one second. for now. todo: compartmentalize into different weapons on the ship.
			Shoot();
		}

	if (thrusting)
	{
		//calculate facing vector
		//float radians = angle * ((float)M_PI / 180.f);
		float radians = glm::radians(angle);
		glm::vec2 facing;
		facing.x = std::cos(radians);
		facing.y = std::sin(radians);

		facing *= seconds * 400.f;

		velocity += facing;

		//check if over max speed
		if (velocity.length() > 900.f)
		{
			velocity = glm::normalize(velocity) * 900.f;
		}

		//animation timing
		thrustTimer += seconds;

		if (thrustTimer >= 1.f / 20.f)
		{
			//change frames
			frameNumber++;
			if (frameNumber > 3)
				frameNumber = 1;

			thrustTimer -= 1.f / 20.f;
		}

	}
	else frameNumber = 0;

	//update position
	position += velocity * seconds;

	//bounds check position
	if (position.x < 0) position.x += 1920.f;
	if (position.x > 1920) position.x -= 1920.f;
	if (position.y < 0) position.y += 1080.f;
	if (position.y > 1080) position.y -= 1080.f;

	//reduce velocity due to "space friction"
	float scale = 1.f - seconds * 0.5f;
	if (scale < 0) scale = 0;
	velocity *= scale;
}

bool Ship::Shoot()
{
	//todo, move towards the presumably compartmentalized weapons on the ship.
	float shot_lifespan = 3.0f;
	Projectile* Proj = new Projectile(this, "Media\\shot.png", 32, 16.0f, shot_lifespan);
	Projectiles.push_back(Proj);
	return true;
}