#include "World.h"

World::World(sf::RenderWindow &window) : m_physic_world(b2World(b2Vec2(0.0f, 0.0f))), m_window(window) ,
										 m_player(Player(m_physic_world, "Antony")), 
										 m_ennemy_player(Player(m_physic_world, "Armen")),
									     thread_receive(&World::receiveFromServer, this)
{
	std::cout << "Taper votre ip :" << std::endl;
	std::cin >> m_ip_adress;
	sf::Socket::Status status = socket.connect(m_ip_adress, 5588);
	if (status != sf::Socket::Done)
	{
		std::cout << "[CLIENT] : Impossible de se connecter au serveur" << std::endl;
	}

	/* On recupère l'id donné par le serveur */
	sf::Packet packet;
	socket.receive(packet);
	packet >> m_id;
	std::cout << "[CLIENT] Mon id : " << m_id << std::endl;


	thread_receive.launch();

	/* On initialise le jeu .... */
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
	m_player.getMovement(m_window);
	m_player.update();
	m_player.draw(m_window);

	m_ennemy_player.update();
	std::cout << "Taille liste arrow ennemy :" << m_ennemy_player.getArrowList().size() << std::endl;
	for (int i = 0; i < m_ennemy_player.getArrowList().size(); i++)
	{
		std::cout << m_ennemy_player.getArrowList()[i]->getPosition().x << " / " << m_ennemy_player.getArrowList()[i]->getPosition().y << std::endl;
	}
	m_ennemy_player.draw(m_window);

	sf::Packet packet;
	packet << m_player;
	socket.send(packet);

	m_physic_world.Step(deltaTime, velocityIterations, positionIterations);
}

void World::receiveFromServer()
{
	while (true)
	{
		int request_id;
		sf::Packet packet;
		socket.receive(packet);
		packet >> request_id;

		if (request_id == m_id)
		{
			m_player.extractDataPlayer(packet);
		}
		else
		{
			m_ennemy_player.extractDataEnnemyPlayer(packet);
		}
	}
}