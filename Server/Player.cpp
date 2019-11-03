#include "Player.h"
#include "CollisionCategorie.h"

Player::Player(b2World& physic_world, std::string name, b2Vec2 position) : Actor(physic_world), m_cadence(5), m_timer_fire(1 /m_cadence), m_timer_stun(1.0f)
{
	m_moveSpeed = 15;
	m_isShooting = false;
	m_isLoading = false;
	m_isStun = false;
	m_name = name;


	/* --- Physics settings --- */

	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(position.x / 30.0f, position.y / 30.0f);

	body = physic_world.CreateBody(&bodyDef);

	dynamicBox.SetAsBox(50.0f / 60.0f, 50.0f / 60.0f);	/* Box sans friction */

	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.0f;
	fixtureDef.filter.categoryBits = ENEMY_PLAYER;
	bodyDef.fixedRotation = true;
	body->CreateFixture(&fixtureDef);
	body->SetUserData(this);

	m_velocity = b2Vec2(0.0f, 0.0f);

	/* ------------------------- */

	m_Texture.loadFromFile("Assets/player1.png");
	m_Texture.setSmooth(true);
	m_Sprite = sf::Sprite(m_Texture, sf::IntRect(0, 0, 50, 50));
	m_size = b2Vec2(50, 50);
	m_Sprite.setOrigin(m_size.x / 2, m_size.y / 2);

	m_TextureAim.loadFromFile("Assets/aiming_trail.png");
	m_TextureAim.setSmooth(true);
	m_SpriteAim = sf::Sprite(m_TextureAim, sf::IntRect(0, 0, 1500, 5));
	m_SpriteAim.setOrigin(0, 2.5);
}

void Player::treatInputs()
{
	for (int i = 0; i < m_input.size(); i++)
	{
		switch (m_input[i])
		{
		case TOP :
			m_velocity.y = -1 * m_moveSpeed;
			break;

		case BOT:
			m_velocity.y = 1 * m_moveSpeed;
			break;

		case LEFT:
			m_velocity.x = -1 * m_moveSpeed;
			break;

		case RIGHT:
			m_velocity.x = 1 * m_moveSpeed;
			break;

		case LEFT_CLICK:
			bullets.push_back(std::make_unique<Arrow>(b2Vec2(m_position.x + 60 * m_mouseDirection.x, m_position.y + 60 * m_mouseDirection.y), m_mouseDirection, m_physic_world, FRIENDLY_ARROW, ENEMY_ARROW | ENEMY_PLAYER, *this));
			break;

		default :
			std::cout << "" << std::endl;
		}
	}

	m_input.clear();
}

std::vector<std::unique_ptr<Arrow>>& Player::getArrowList()
{
	return bullets;
}

std::vector<std::unique_ptr<LetalShoot>>& Player::getLetalList()
{
	return letals;
}

b2Vec2 Player::getMouseDirection()
{
	return m_mouseDirection;
}

void Player::update()
{
	Actor::update();

	float clamping = 0.75;
	m_moveSpeed = 15;
	b2Vec2 new_velocity = body->GetLinearVelocity(); /* Variable utilisée seulement si le joueur est stun */ 

	m_Sprite.setRotation(90 - atan2(m_mouseDirection.x, m_mouseDirection.y) * 180 / b2_pi);


	if (m_isShooting)
	{
		m_moveSpeed = 0.35f * m_moveSpeed;
	}
		
	if (m_timer_stun.isOver())
	{
		m_isStun = false;
	}

	if (m_isLoading)
	{
		m_loadingSprite.update(sf::Vector2f(m_position.x, m_position.y));
	}
	else
	{
		m_loadingSprite.reset();
	}

	/* Update des projectiles classiques */
	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i]->update();
	}

	/* Update des letals shoots */
	for (int i = 0; i < letals.size(); i++)
	{
		letals[i]->update();
	}


	if (m_isStun) // On laisse un peu plus d'inertie au player si il est stun (pour le bump)
	{
		clamping = 0.85;
	}

	/* Alternative de clamping au lieu de la friction */
	m_velocity.x >= -0.01 && m_velocity.x <= 0.01 ? m_velocity.x = 0 : m_velocity.x = clamping * m_velocity.x; /* Si jamais la velocity est petite */
	m_velocity.y >= -0.01 && m_velocity.y <= 0.01 ? m_velocity.y = 0 : m_velocity.y = clamping * m_velocity.y; /* on la remet à 0				   */
	body->SetLinearVelocity(m_velocity);

}

void Player::draw(sf::RenderTarget &window)
{
	Actor::draw(window);
	
	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i]->draw(window);
	}

	for (int i = 0; i < letals.size(); i++)
	{
		letals[i]->draw(window);
	}

	if (m_isLoading)
	{
		m_loadingSprite.draw(window);
		m_SpriteAim.setPosition(m_position.x, m_position.y);
		m_SpriteAim.setRotation(90 - atan2(m_mouseDirection.x, m_mouseDirection.y) * 180 / b2_pi);
		window.draw(m_SpriteAim);
	}
}

void Player::bump(b2Vec2 velocity, float force)
{
	m_isStun = true;
	m_timer_stun.refresh();
	m_velocity = b2Vec2(velocity.x * force, velocity.y * force);
}

bool Player::isAlive()
{
	return m_isAlive;
}

void Player::printInputs()
{
	for (int i = 0; i < m_input.size(); i++)
	{
		//std::cout << m_input[i] << std::endl;
	}
}

void operator >>(sf::Packet& packet, Player& character)
{
	character.m_input.clear();

	if (!packet.endOfPacket())
	{
		packet >> character.m_mouseDirection.x;
	}
	if (!packet.endOfPacket())
	{
		packet >> character.m_mouseDirection.y;
	}
	if (!packet.endOfPacket())
	{
		packet >> character.m_isShooting;
	}
	if (!packet.endOfPacket())
	{
		packet >> character.m_isLoading;
	}

	int input_tmp;
	while (!packet.endOfPacket())
	{
		packet >> input_tmp;
		character.m_input.push_back(static_cast<Input>(input_tmp));
	}
}