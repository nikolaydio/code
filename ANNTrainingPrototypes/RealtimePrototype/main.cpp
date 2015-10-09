#include "main.h"
#include <iostream>

#define PI 3.14159265
#define DIST_CONST 325
#define SPEED 10
#define ROT_SPEED 30
#define MOV_SPEED 20
const float MISSLE_SPEED = 300;
const float TIME_ALIVE = 30;
Game::Game() : window(sf::VideoMode(640, 480), "ANN Realtime Training Prototype"), target(20), character(sf::Vector2f(70,30))
{
}

Missle::Missle(sf::Vector2f pos, float rot) : missle(sf::Vector2f(20, 5))
{
	missle.setOrigin(10,2.5);
	missle.setFillColor(sf::Color(0,0,0,255));
	missle.setRotation(rot);
	missle.setPosition(pos);
	timePassed = 0;
}
bool Missle::Update(float deltaTime)
{
	//move the missle
	float rot = missle.getRotation();
	sf::Vector2f move;
	move.x = cos(rot*PI/180) * MISSLE_SPEED * deltaTime;
	move.y = sin(rot*PI/180) * MISSLE_SPEED * deltaTime;
	missle.move(move.x, move.y);

	timePassed += deltaTime;
	if(timePassed > TIME_ALIVE)
	{
		return false;
	}
	return true;
}
void Missle::Render(sf::RenderWindow* window)
{
	window->draw(missle);
}


void Game::Initialize()
{
	target.setFillColor(sf::Color(255,0,0));
	character.setFillColor(sf::Color(0,0,255));
	state = S_WANDERING;
	pstate = S_PLAYING;
	character.setOrigin(35,15);
	target.setOrigin(20,20);
	//brain = MN_CreateBrain(4, 2.0, 5, 1.25);
	brain = MN_CreateBrain(3, 2.0, 6, 1.65);
	MN_SetNodeCount(brain, 0, 3);
	MN_SetNodeCount(brain, 1, 10);
	//MN_SetNodeCount(brain, 2, 6);
	MN_SetNodeCount(brain, 2, 4);
	MN_AllocateBrainConnections(brain, 1);
}
void Game::MainLoop()
{
	while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
			else if(event.type == sf::Event::KeyPressed)
			{
				if(event.key.code == sf::Keyboard::T)
				{
					if(pstate == S_TEACHING)
					{
						pstate = S_PLAYING;
					}else if(pstate == S_PLAYING)
					{
						pstate = S_TEACHING;
					}
				}
				//load from file
				if(event.key.code == sf::Keyboard::Y)
				{
					if(pstate == S_TEACHING)
					{
						char filename[32];
						std::cin >> filename;
						if(MN_LoadBrainFromFile(brain, filename) != 0)
						{
							std::cout << "Brain loaded." << std::endl;
						}
						else
						{
							std::cout << "Brain load failed." << std::endl;
						}
						timer.restart();
					}
				}
				if(event.key.code == sf::Keyboard::U)
				{
					if(pstate == S_TEACHING)
					{
						char filename[32];
						std::cin >> filename;
						if(MN_SaveBrainToFile(brain, filename) != 0)
						{
							std::cout << "Brain saved." << std::endl;
						}
						else
						{
							std::cout << "Brain save failed." << std::endl;
						}
						timer.restart();
					}
				}
			}
        }

		sf::Time Time = timer.getElapsedTime();
		timer.restart();
		time = Time.asSeconds();

		RunEntities();
		RunTarget();
		RunPlayer();

        window.clear(sf::Color(255,255,255));
        window.draw(target);
		window.draw(character);
		RenderEntities();
        window.display();
		_sleep(10);
    }
}

void Game::RunPlayer()
{
	//update input
	// 0 = distance to target
	sf::Vector2f diff = target.getPosition() - character.getPosition();
	float dist = sqrt(diff.x * diff.x + diff.y * diff.y), dist_val = 0;
	if(dist > 0.001f)
	{
		dist_val = dist / DIST_CONST;
	}
	//clap at one
	if(dist_val > 1)
	{
		dist_val = 1;
	}
	MN_SetBrainInput(brain, 0, dist_val);

	// 2 = orientation according to the target
	sf::Vector2f ori; float rot = character.getRotation(); float dot;
	if(dist >= 0.01f)
	{
		diff.x /= dist;
		diff.y /= dist;
	}
	ori.x = cos((rot+90)*PI/180);
	ori.y = sin((rot+90)*PI/180);
	//both are normalized now, find dot
	dot = ori.x * diff.x + ori.y * diff.y;
	MN_SetBrainInput(brain, 1, (dot));


	ori.x = cos((rot)*PI/180);
	ori.y = sin((rot)*PI/180);
	dot = ori.x * diff.x + ori.y * diff.y;
	MN_SetBrainInput(brain, 2, (dot));

	//update brain
	MN_UpdateBrain(brain);
	if(pstate == S_PLAYING)
	{
		float result[4] = { 0,0,0, 0 };
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			sf::Vector2f move;
			float rot = character.getRotation();
			move.x = cos(rot*PI/180);
			move.y = sin(rot*PI/180);
			character.move(move.x*time*SPEED*MOV_SPEED, move.y*time*SPEED*MOV_SPEED);
			result[0] = 1;
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			character.rotate(ROT_SPEED*SPEED*time);
			result[1] = 1;
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			character.rotate(-ROT_SPEED*SPEED*time);
			result[2] = 1;
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			entity_list.push_back(new Missle(character.getPosition(), character.getRotation()));
			result[3] = 1;
		}
		//train the brain
		std::cout << MN_BrainAdjust(brain, result) << std::endl;
	}else if(pstate == S_TEACHING)
	{
		float test[4] = { MN_GetBrainOutput(brain, 0), MN_GetBrainOutput(brain, 1),
			MN_GetBrainOutput(brain, 2), MN_GetBrainOutput(brain, 3) };
		if(test[0] > 0.76f)
		{
			sf::Vector2f move;
			float rot = character.getRotation();
			move.x = cos(rot*PI/180);
			move.y = sin(rot*PI/180);
			character.move(move.x*time*SPEED*MOV_SPEED, move.y*time*SPEED*MOV_SPEED);
		}
		if(test[1] > 0.76f)
		{
			character.rotate(ROT_SPEED*SPEED*time);
		}
		if(test[2] > 0.76f)
		{
			character.rotate(-ROT_SPEED*SPEED*time);
		}
		if(test[3] > 0.76f)
		{
			entity_list.push_back(new Missle(character.getPosition(), character.getRotation()));
		}
		//std::cout << "1. " << test[0] << "2. " << test[1] << "3. " << test[2] << std::endl;
	}
}

void Game::RunTarget()
{
		//run target ai
		if(state == S_WANDERING)
		{
			//create a new target and set to moving
			target_loc = sf::Vector2f(rand() % 640, rand() % 480);
			state = S_MOVING;
		}else if(state == S_MOVING)
		{
			sf::Vector2f move = (target_loc - target.getPosition()) ;
			float len = sqrt(move.x * move.x + move.y * move.y);
			move.x /= len;
			move.y /= len;
			move.x *= time * 25 * SPEED;
			move.y *= time * 25 * SPEED;
			target.move(move.x, move.y);
			move = target.getPosition() - target_loc;
			len = sqrt(move.x * move.x + move.y * move.y);
			if(len < 100)
			{
				state = S_WANDERING;
			}
		}
}


void Game::RunEntities()
{
	std::list<Entity*>::iterator i = entity_list.begin();
	std::list<Entity*>::iterator end = entity_list.end();
	while(i != end)
	{
		if(!(*i)->Update(time))
		{
			i = entity_list.erase(i);
			continue;
		}
		++i;
	}
}
void Game::RenderEntities()
{
	std::list<Entity*>::iterator i = entity_list.begin();
	std::list<Entity*>::iterator end = entity_list.end();
	while(i != end)
	{
		(*i)->Render(&window);
		++i;
	}
}
void Game::ShutDown()
{
}


int main()
{
    Game game;
	game.Initialize();
	game.MainLoop();
	game.ShutDown();
    return 0;
}