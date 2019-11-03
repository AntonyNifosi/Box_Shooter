#include "World.h"

World::World(sf::RenderWindow &window) : m_physic_world(b2World(b2Vec2(0.0f, 0.0f))), m_window(window) ,
										 m_player1(Player(m_physic_world, "Antony", b2Vec2(200.0f, 100.0f))), 
										 m_player2(Player(m_physic_world, "Armen", b2Vec2(800.0f, 700.0f))),
										 thread_client1(&World::receiveClient1, this),
										 thread_client2(&World::receiveClient2, this)
{
	

	// lie l'écouteur à un port
	if (listener.listen(5588) != sf::Socket::Done)
	{
		std::cout << "[SERVEUR] : Impossible d'ecouter sur ce port" << std::endl;
	}

	// Accepte le client 1
	
	if (listener.accept(client1) != sf::Socket::Done)
	{
		std::cout << "[SERVEUR] : Impossible d'accepter le client 1 " << std::endl;
	}
	else
	{
		sf::Packet packet;
		packet << 1;
		client1.send(packet);
	}

	// Accepte le client 1
	if (listener.accept(client2) != sf::Socket::Done)
	{
		std::cout << "[SERVEUR] : Impossible d'accepter le client 2 " << std::endl;
	}
	else
	{
		sf::Packet packet;
		packet << 2;
		client2.send(packet);
	}

	
	/* Une fois qu'on a accepté les deux clients on lances les threads */

	thread_client1.launch();
	thread_client2.launch();

	clock.restart();
	velocityIterations = 6;
	positionIterations = 2;
	m_physic_world.SetContactListener(&myContactListenerInstance);
	createEnvironnement();
}


void World::createEnvironnement()
{
	/* ----------------------------------------------------------------- */

	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, 805.0f / 30.0f);

	b2Body* groundBody = m_physic_world.CreateBody(&groundBodyDef);

	b2PolygonShape groundBox;
	groundBox.SetAsBox(2000 / 60.0f, 10.0f / 60.0f);

	groundBody->CreateFixture(&groundBox, 0.0f);

	/* -------------- Left wall -------------- */

	b2BodyDef leftwallBodyDef;
	leftwallBodyDef.position.Set(-5.0f / 30.0f, 0.0f);

	b2Body* leftwallBody = m_physic_world.CreateBody(&leftwallBodyDef);

	b2PolygonShape leftwallBox;
	leftwallBox.SetAsBox(10.0f / 60.0f, 2000.0f / 60.0f);

	leftwallBody->CreateFixture(&leftwallBox, 0.0f);

	/* -------------- Right wall -------------- */

	b2BodyDef rightwallBodyDef;
	rightwallBodyDef.position.Set(805.0f / 30.0f, 0.0f);

	b2Body* rightwallBody = m_physic_world.CreateBody(&rightwallBodyDef);

	b2PolygonShape rightwallBox;
	rightwallBox.SetAsBox(10.0f / 60.0f, 2000.0f / 60.0f);

	rightwallBody->CreateFixture(&rightwallBox, 0.0f);

	/* -------------- Sky -------------- */

	b2BodyDef skyBodyDef;
	skyBodyDef.position.Set(0.0f, -5.0f / 30.0f);

	b2Body* skyBody = m_physic_world.CreateBody(&skyBodyDef);

	b2PolygonShape skyBox;
	skyBox.SetAsBox(2000 / 60.0f, 10.0f / 60.0f);

	skyBody->CreateFixture(&skyBox, 0.0f);
}

void World::update()
{
	deltaTime = clock.restart().asSeconds();

	m_player1.treatInputs();
	m_player1.update();
	m_player1.draw(m_window);

	m_player2.treatInputs();
	m_player2.update();
	m_player2.draw(m_window);

	m_physic_world.Step(deltaTime, velocityIterations, positionIterations);

	sendAllClients();
}

void World::receiveClient1()
{
	while (true)
	{
		/* On recçoit le paquet du client 1 */
		sf::Packet packet;
		client1.receive(packet);

		packet >> m_player1;

		m_player1.printInputs();
	}	
}

void World::receiveClient2()
{
	while (true)
	{
		sf::Packet packet;
		client2.receive(packet);

		packet >> m_player2;

		m_player2.printInputs();
	}
}

void World::sendAllClients()
{
	if (!m_physic_world.IsLocked())
	{
		/* ------------------------ CLIENT 1 ------------------------- */
		sendToClient1();

		/* ------------------------ CLIENT 2 ------------------------- */
		sendToClient2();
	}
}

void World::sendToClient1()
{
	/* On envoie les données de la position du client 1 au client 1*/
	sf::Packet c1_packet_to_send1;
	c1_packet_to_send1 << 1;
	b2Vec2 client1_position = m_player1.getPosition();
	c1_packet_to_send1 << client1_position.x << client1_position.y;

	client1.send(c1_packet_to_send1);

	/* On envoie les données de la position du client 2 au client 1*/
	sf::Packet c1_packet_to_send2;
	c1_packet_to_send2 << 2;
	b2Vec2 client2_position = m_player2.getPosition();
	c1_packet_to_send2 << client2_position.x << client2_position.y;
	b2Vec2 mouseDirection = m_player2.getMouseDirection();
	c1_packet_to_send2 << mouseDirection.x << mouseDirection.y;


	std::vector<std::unique_ptr<Arrow>>& bullets = m_player2.getArrowList();
	for (int i = 0; i < bullets.size(); i++)
	{
		if (!bullets[i]->isSend())
		{
			c1_packet_to_send2 << bullets[i]->getPosition().x << bullets[i]->getPosition().y;
			//std::cout << "On envoie la position :" << bullets[i]->getPosition().x << " / " << bullets[i]->getPosition().y << std::endl;
			c1_packet_to_send2 << bullets[i]->getVelocity().x << bullets[i]->getVelocity().y;
			bullets[i]->setisSend(true);
		}
	}

	client1.send(c1_packet_to_send2);
}

void World::sendToClient2()
{
	sf::Packet c2_packet_to_send1;
	c2_packet_to_send1 << 2;
	b2Vec2 client2_position = m_player2.getPosition();
	c2_packet_to_send1 << client2_position.x << client2_position.y;

	client2.send(c2_packet_to_send1);


	sf::Packet c2_packet_to_send2;
	c2_packet_to_send2 << 1;
	b2Vec2 client1_position = m_player1.getPosition();
	c2_packet_to_send2 << client1_position.x << client1_position.y;
	b2Vec2 mouseDirection = m_player1.getMouseDirection();
	c2_packet_to_send2 << mouseDirection.x << mouseDirection.y;



	std::vector<std::unique_ptr<Arrow>>& bullets = m_player1.getArrowList();
	for (int i = 0; i < bullets.size(); i++)
	{
		if (!bullets[i]->isSend())
		{
			c2_packet_to_send2 << bullets[i]->getPosition().x << bullets[i]->getPosition().y;
			std::cout << "On envoie la position :" << bullets[i]->getPosition().x << " / " << bullets[i]->getPosition().y << std::endl;
			c2_packet_to_send2 << bullets[i]->getVelocity().x << bullets[i]->getVelocity().y;
			bullets[i]->setisSend(true);
		}
	}
	client2.send(c2_packet_to_send2);
}
