#include "World.h"
#include "ContactListener.h"
#include <iostream>


int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 800), "Box Shooting Client");
	window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(true);


	sf::Texture image_background;
	sf::Sprite background;
	image_background.loadFromFile("Assets/background_blue.jpg");
	background = sf::Sprite(image_background, sf::IntRect(0, 0, 1920, 1080));

	World world(window);


	sf::TcpSocket socket;
	



	while (window.isOpen())
	{
		sf::Event event;

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear(sf::Color(240, 240, 240));
		window.draw(background);
		world.update();
		window.display();
	}
	return 0;
}