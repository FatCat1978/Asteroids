#include "Projectile.h"
#include "Asteroids.h"
#include <math.h>
extern Blit3D* blit3D;
extern std::vector<Projectile*> Projectiles;
extern std::vector<Asteroid*> Asteroids;


void Projectile::Draw()
{
	//change ship angle because my graphics face "up", not "right"
	shotSprite->angle = angle - 90;

	//draw main proj
	shotSprite->Blit(position.x, position.y);

	//redraw if too close to an edge
	//left
	if (position.x < radius + 10.f)
		shotSprite->Blit(position.x + 1920.f, position.y);
	//right
	if (position.x > 1920.f - (radius + 10.f))
			shotSprite->Blit(position.x - 1920.f, position.y);
	//down
	if (position.y < radius + 10.f)
		shotSprite->Blit(position.x, position.y + 1080.f);
	//up
	if (position.y > 1080.f - (radius + 10.f))
		shotSprite->Blit(position.x, position.y - 1080.f);

	//copies for 4 diagonal corners
	shotSprite->Blit(position.x + 1920.f, position.y + 1080.f);
	shotSprite->Blit(position.x - 1920.f, position.y - 1080.f);
	shotSprite->Blit(position.x - 1920.f, position.y + 1080.f);
	shotSprite->Blit(position.x + 1920.f, position.y - 1080.f);
}


bool Projectile::Update(float seconds)
{
	if (queue_for_del)
		return true;
	position += (velocity) * seconds;
	lifespan -= seconds;
	if (lifespan <= 0)
		queue_for_del = true;
	/*
		glm::vec2 AsteroidDistVector = ship->position - position;
	float dist = sqrt(pow(AsteroidDistVector.x, 2) + pow(AsteroidDistVector.y, 2));
	if (dist < (radius + ship->radius))
		on_contact_ship();*/
	if (position.x < 0) position.x += 1920.f;
	if (position.x > 1920) position.x -= 1920.f;
	if (position.y < 0) position.y += 1080.f;
	if (position.y > 1080) position.y -= 1080.f;

	return ((lifespan <= 0));
}


Projectile::Projectile(Ship* owner, std::string media, int iconsize, float nradius, float slifespan) //constructor.
{
	int modifier = 3000;
	lifespan = slifespan;
	shotSprite = blit3D->MakeSprite(0, 0, iconsize, iconsize, media);

	float newangle = owner->angle;

	angle = newangle;

	position = owner->position;
	velocity = owner->velocity;

	glm::vec2 momentum;
	newangle = glm::radians(newangle);
	momentum.x = modifier*cos(newangle);
	momentum.y = modifier*sin(newangle);
	
	velocity += momentum;


}


