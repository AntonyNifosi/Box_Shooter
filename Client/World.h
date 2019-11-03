#pragma once

#ifndef H_WORLD
#define H_WORLD

#include "Actor.h"
#include "Player.h"
#include "ContactListener.h"

class World
{
private:
	int m_id;

	b2World m_physic_world;
	MyContactListener myContactListenerInstance; 
	/*std::vector<Actor> tiles;*/
	Player m_player;
	Player m_ennemy_player;
	sf::RenderWindow &m_window;
	sf::Clock clock;
	float deltaTime;

	int32 velocityIterations;
	int32 positionIterations;

	std::string m_ip_adress;
	sf::TcpSocket socket;
	sf::Thread thread_receive;

public:
	World(sf::RenderWindow &window);
	void createEnvironnement();
	void update();
	void receiveFromServer();
};

#endif