#pragma once
#include <SFML/Graphics.hpp>
#include <brain.h>
#include <list>

enum STATE
{
	S_WANDERING,
	S_MOVING
};
enum PLAYER_STATE
{
	S_PLAYING,
	S_TEACHING
};
class Entity
{
public:
	Entity() {}
	~Entity() {}

	virtual bool Update(float deltaTime) = 0;
	virtual void Render(sf::RenderWindow* window) = 0;
};
class Missle : public Entity
{
public:
	Missle(sf::Vector2f pos, float rot);
	bool Update(float deltaTime);
	void Render(sf::RenderWindow* window);

	sf::RectangleShape missle;
	float timePassed;
};
class Game
{
public:
	Game();
	~Game() {}

	void Initialize();
	void MainLoop();
	void ShutDown();
	
	void RenderEntities();

	void RunTarget();
	void RunPlayer();
	void RunEntities();

	STATE state;
	PLAYER_STATE pstate;
	sf::Vector2f target_loc;
	sf::CircleShape target;
	sf::RectangleShape character;
	sf::RenderWindow window;
	float time;
	sf::Clock timer;
	MN_Brain* brain;
	std::list<Entity*> entity_list;
};