#pragma once

#ifndef H_WORLD
#define H_WORLD

#include "Actor.h"
#include "Player.h"
#include "ContactListener.h"

class World
{
private:

	b2World m_physic_world;
	MyContactListener myContactListenerInstance;
	/*std::vector<Actor> tiles;*/
	Player m_player1;
	Player m_player2;
	sf::RenderWindow &m_window;
	sf::Clock clock;
	float deltaTime;

	int32 velocityIterations;
	int32 positionIterations;

	sf::TcpListener listener;

	sf::TcpSocket client1;
	sf::TcpSocket client2;

	sf::Thread thread_client1;
	sf::Thread thread_client2;


public:
	World(sf::RenderWindow &window);
	void createEnvironnement();
	void update();
	void receiveClient1();
	void receiveClient2();
	void sendAllClients();
	void sendToClient1();
	void sendToClient2();
};

#endif