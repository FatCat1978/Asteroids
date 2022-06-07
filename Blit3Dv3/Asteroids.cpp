#include "Asteroids.h"
#include "Ship.h"
#include <random>
extern Blit3D* blit3D;
extern Ship *ship;
extern std::vector<Asteroid*> Asteroids;
extern boolean lossstate;

Asteroid::Asteroid(int asteroidsize, glm::vec2 initial_velocity)
{
	if (queue_delete)
		return;
	size = asteroidsize;
	velocity = initial_velocity;

	if(size == 1)
	{
		AsteroidSprite = blit3D->MakeSprite(0, 0, 32, 32, "Media\\small.png");
		radius = 23;
	}
	if (size == 2)
	{
		AsteroidSprite = blit3D->MakeSprite(0, 0, 64, 64, "Media\\medium.png");
		radius = 24;
	}
	
	if (size == 3)
	{
		AsteroidSprite = blit3D->MakeSprite(0, 0, 96, 96, "Media\\large.png");
		radius = 49;
	}
}

void Asteroid::on_contact_ship()
{
	if (size > 1)
	{
		ship->lives--;
		score -= 150;
		if (ship->lives <= 0)
		{
			lossstate = true;
		}
	}
	else
	{
		ship->score += 1000;
	}
	queue_delete = true;
}

/*
-roll a random number between 0-360
-covert that number to radians
velocity.x = cos(that random radian number)
velocity.y = sin(that random radian number)
//scale up the velocity by multiplying by the base velocity
velocity *= baseVelocity;
//scale it up further if it's a medium or small asteroid
velocity *= 1.5f;
*/
extern std::mt19937 rng;
glm::vec2 randomize_from_impact(glm::vec2 velocityProj, glm::vec2 velocityAst, float powertransfer)
{
	velocityProj *= powertransfer;
	glm::vec2 returned = velocityProj + velocityAst;
	
	std::uniform_int_distribution<unsigned int> uint_dist_angles(0, 360);
	int random = uint_dist_angles(rng);

	glm::vec2 modifier_velocity;
	modifier_velocity.x = sin(random);
	modifier_velocity.y = cos(random);

	returned *= modifier_velocity;

	return returned;
}

void Asteroid::collision(glm::vec2 velocityHitBy) //velocity of colliding proj.
{	
	ship->score += 50 * size;
	if ((size > 1))
	{
		for (int i = 0; i < 2; i++)
		{
			glm::vec2 velocityNew = randomize_from_impact(velocityHitBy, velocity, 0.1);
			Asteroid* hnnng = new Asteroid(size-1, velocityNew);
			hnnng->position = position;

			Asteroids.push_back(hnnng);
		}
	}
	queue_delete = true;

	//shotgun spread from impact point
	
	}

void Asteroid::on_despawn()
{
}

void Asteroid::on_spawn()
{
}

void Asteroid::Draw() //todo, put this on a singlular "base" class.
{
	//change ship angle because my graphics face "up", not "right"
	AsteroidSprite->angle = angle - 90;

	//draw main proj
	AsteroidSprite->Blit(position.x, position.y);

	//redraw if too close to an edge
	//left
	if (position.x < radius + 10.f)
		AsteroidSprite->Blit(position.x + 1920.f, position.y);
	//right
	if (position.x > 1920.f - (radius + 10.f))
		AsteroidSprite->Blit(position.x - 1920.f, position.y);
	//down
	if (position.y < radius + 10.f)
		AsteroidSprite->Blit(position.x, position.y + 1080.f);
	//up
	if (position.y > 1080.f - (radius + 10.f))
		AsteroidSprite->Blit(position.x, position.y - 1080.f);

	//copies for 4 diagonal corners
	AsteroidSprite->Blit(position.x + 1920.f, position.y + 1080.f);
	AsteroidSprite->Blit(position.x - 1920.f, position.y - 1080.f);
	AsteroidSprite->Blit(position.x - 1920.f, position.y + 1080.f);
	AsteroidSprite->Blit(position.x + 1920.f, position.y - 1080.f);
}

bool Asteroid::Update(float seconds)
{   
    position += (velocity)*seconds;
	if (position.x < 0) position.x += 1920.f;
	if (position.x > 1920) position.x -= 1920.f;
	if (position.y < 0) position.y += 1080.f;
	if (position.y > 1080) position.y -= 1080.f;

	if (position.y > 2000) //uh oh
		position.y = 1999;
	if (position.y < -2000)
		position.y = -2000;
	if (position.x > 2000) //uh oh
		position.x = 1999;
	if (position.x < -2000)
		position.x = -2000;
	return true;

}
