//memory leak detection
#define CRTDBG_MAP_ALLOC
#ifdef _DEBUG
	#ifndef DBG_NEW
		#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
		#define new DBG_NEW
	#endif
#endif  // _DEBUG

#include <random>
#include <stdlib.h>
#include <crtdbg.h>

#include "Blit3D.h"

#include "Projectile.h"
#include "Ship.h"
#include "Asteroids.h"

glm::vec2 randomize_from_spawn();

Blit3D *blit3D = NULL;
AngelcodeFont* EnterCommand_Font = NULL;

float time_between_level = 3; //seconds

boolean lossstate = false;
std::mt19937 rng;
float time_since_loss = 0;

//GLOBAL DATA
double elapsedTime = 0;
float timeSlice = 1.f / 120.f;

Sprite *backgroundSprite = NULL; //a pointer to a background sprite
Ship *ship = NULL;
int level = 0;
std::vector<Projectile*> Projectiles; //collection of projectiles.
std::vector<Asteroid*> Asteroids; //asteroids.
void next_level()
{
	time_between_level = 3;
	level++;
	//determine where player is
	//introduce asteroids on far side of the screen, random height
	bool left = (ship->position.x < (1920 / 2));
	float spawnx = 120;
	if (!left)
		spawnx = 1800;

	std::uniform_int_distribution<unsigned int> uint_dist_height(0, 1080);
	std::uniform_int_distribution<unsigned int> uint_dist_spawnvariance(0, 200);
	//spawn on the right
	for (int i = 0; i < (level*4); i++)
	{
		glm::vec2 velocityNew = randomize_from_spawn();
		Asteroid* hnnng = new Asteroid(3, velocityNew);
		glm::vec2 newpos;
		newpos.y = 0.f + uint_dist_height(rng);
		newpos.x = spawnx + (uint_dist_spawnvariance(rng)-100);
		hnnng->position = newpos;
		Asteroids.push_back(hnnng);
	}
		
	
}
boolean generic_collision_check(float radA, glm::vec2 posA, float radB, glm::vec2 posB)
{
		float dist = 1000000000;
		glm::vec2 DistVector = posA - posB;
		dist = sqrt(pow(DistVector.x, 2) + pow(DistVector.y, 2));
		float radius_check = (radA + radB);
		if (dist <= radius_check)
			return true;
		return false;
}

void restart_game()
{
	ship->position = glm::vec2(1920.f / 2, 1080.f / 2);
	//loop through asteroid list & clear
	for (int a = Asteroids.size() - 1; a >= 0; a--) //go backwards to prevent crashes.
	{
		delete(Asteroids[a]);
		Asteroids.erase(Asteroids.begin() + a);
	}
	//loop through Proj List and Clear
	for (int p = Projectiles.size() - 1; p >= 0; p--) //go backwards to prevent crashes.
	{
		delete(Projectiles[p]);
		Projectiles.erase(Projectiles.begin() + p);
	}
	ship->lives = 3;
	ship->score = 0;
	level = 1;
	lossstate = false;
}

void Init()
{
	int seedVal;
	std::random_device rd; //not as good/fast as std::mt19937, but a good unpredictable sequence starter.
	seedVal = rd();
	rng.seed(seedVal);
	//turn cursor off
	blit3D->ShowCursor(false);

	//load our background image: the arguments are upper-left corner x, y, width to copy, height to copy, and file name.
	backgroundSprite = blit3D->MakeSprite(0, 0, 1920, 1080, "Media\\back.png");

	//create a ship
	EnterCommand_Font = blit3D->MakeAngelcodeFontFromBinary32("Media\\Fonts\\EnterCommand_Font\\EnterCommand_Font.bin");
	ship = new Ship;
	//load a sprite off of a spritesheet
	for (int i = 0; i < 4; ++i)
		ship->spriteList.push_back(blit3D->MakeSprite(i * 72, 0, 72, 88, "Media\\Player_Ship2c.png"));

	ship->position = glm::vec2(1920.f / 2, 1080.f / 2);

	/*glm::vec2 AsteroidVel(10, 10);
	Asteroid *hnnng = new Asteroid(3, AsteroidVel);
	hnnng->position.x = 1920 / 5;
	hnnng->position.y = 1080 / 4;
	Asteroids.push_back(hnnng);
	//hardcode pos for now
	*/
	//set the clear colour
	glClearColor(1.0f, 0.0f, 1.0f, 0.0f);	//clear colour: r,g,b,a
}

void DeInit(void)
{
	if(ship != NULL) delete ship;
	//any sprites/fonts still allocated are freed automatically by the Blit3D object when we destroy it
}

void Update(double seconds)
{
	if (lossstate)
	{
		time_since_loss += seconds;
		if (time_since_loss > 3)
			time_since_loss = 3;
		return; //pause the game if dead
	}

	//only update time to a maximun amount - prevents big jumps in 
	//the simulation if the computer "hiccups"
	if (seconds < 0.15)
		elapsedTime += seconds;
	else elapsedTime += 0.15;

	//update by a full timeslice when it's time
	while (elapsedTime >= timeSlice)
	{
		elapsedTime -= timeSlice;
		ship->Update(timeSlice);
		
		for (auto a : Asteroids)
		{
			a->Update(timeSlice);
		}
		for (int p = Projectiles.size() - 1; p >= 0; p--) //go backwards to prevent crashes.
		{
			Projectiles[p]->Update(timeSlice);
			if (Projectiles[p]->queue_for_del)
			{
				delete(Projectiles[p]);
				Projectiles.erase(Projectiles.begin() + p);
			}
		}


		for (int a = Asteroids.size() - 1; a >= 0; a--) //go backwards to prevent crashes.
		{
			
			if (generic_collision_check(Asteroids[a]->radius, Asteroids[a]->position, ship->radius, ship->position))
			{
				Asteroids[a]->on_contact_ship();
			}
			if (Asteroids[a]->queue_delete)
			{
				delete(Asteroids[a]);
				Asteroids.erase(Asteroids.begin() + a);
				continue;
			}
			for (int i = Projectiles.size() - 1; i >= 0; --i) //go backwards to prevent crashes.
			{
				if(generic_collision_check(Asteroids[a]->radius, Asteroids[a]->position, Projectiles[i]->radius, Projectiles[i]->position))
				{
				Projectiles[i]->queue_for_del = true;
				Asteroids[a]->collision(Projectiles[i]->velocity);
				Asteroids[a]->queue_delete = true;
				}

				if (Asteroids[a]->queue_delete)
				{
					delete(Asteroids[a]);
					Asteroids.erase(Asteroids.begin() + a);
					break;
				}
				if (Projectiles[i]->queue_for_del)
				{
					Projectile* idiot = Projectiles[i];
					Projectiles.erase(Projectiles.begin() + i);
					delete(idiot); //need this for making sure there's no leaks.
				}
			}
			

		}
		if (Asteroids.size() == 0)
		{
			time_between_level -= timeSlice;
		}
		if (time_between_level <= 0)
			next_level();


	}
}

void Draw(void)
{
	// wipe the drawing surface clear

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//draw stuff here

	//draw the background in the middle of the screen
	//the arguments to Blit(0 are the x, y pixel coords to draw the center of the sprite at, 
	//starting as 0,0 in the bottom-left corner.
	//draw the ship
	backgroundSprite->Blit(1920.f / 2, 1080.f / 2);
	ship->Draw();
	for (auto& p : Projectiles)
		p->Draw();
	for (auto& a : Asteroids)
		a->Draw();
	std::string meme = "HP:" + std::to_string(ship->lives);

	std::string health = "SCORE:" + std::to_string(ship->score);
	float width = EnterCommand_Font->WidthText(health);
	//score update
	EnterCommand_Font->BlitText((1920.f / 20), (1080.f / 20), meme); //probably won't scale correctly. fix in future.
	EnterCommand_Font->BlitText((1920.f / 20) * 19-width, (1080.f / 20), health);
	if(lossstate)
		EnterCommand_Font->BlitText(1920 / 2-(EnterCommand_Font->WidthText("GAME OVER. TRY AGAIN?")/2), 1080 / 2, "GAME OVER. TRY AGAIN?");
}

//the key codes/actions/mods for DoInput are from GLFW: check its documentation for their values
void DoInput(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		blit3D->Quit(); //start the shutdown sequence

	if (lossstate)
	{
		if(time_since_loss > 2)
			restart_game();
		return;
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		ship->isFiring = true;

	if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
		ship->isFiring = false;

	if (key == GLFW_KEY_A && action == GLFW_PRESS)
		ship->turningLeft = true;

	if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		ship->turningLeft = false;

	if (key == GLFW_KEY_D && action == GLFW_PRESS)
		ship->turningRight = true;

	if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		ship->turningRight = false;

	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		ship->thrusting = true;

	if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		ship->thrusting = false;

}

int main(int argc, char *argv[])
{
	//memory leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//set X to the memory allocation number in order to force a break on the allocation:
	//useful for debugging memory leaks, as long as your memory allocations are deterministic.
	//_crtBreakAlloc = X;

	blit3D = new Blit3D(Blit3DWindowModel::BORDERLESSFULLSCREEN_1080P, 1920, 1080);

	//set our callback funcs
	blit3D->SetInit(Init);
	blit3D->SetDeInit(DeInit);
	blit3D->SetUpdate(Update);
	blit3D->SetDraw(Draw);
	blit3D->SetDoInput(DoInput);

	//Run() blocks until the window is closed
	blit3D->Run(Blit3DThreadModel::SINGLETHREADED);
	delete(ship);
	if (blit3D) delete blit3D;
}


glm::vec2 randomize_from_spawn()
{
	std::uniform_int_distribution<unsigned int> uint_dist_angles(0, 360);
	int random = uint_dist_angles(rng);

	glm::vec2 modifier_velocity;
	modifier_velocity.x = sin(random);
	modifier_velocity.y = cos(random);

	return modifier_velocity;
}

