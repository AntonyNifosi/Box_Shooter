#include "LetalShoot.h"
#include "Player.h"
LetalShoot::LetalShoot(b2Vec2 position, b2Vec2 velocity, b2World& physic_world, uint16 categoryBits, uint16 maskBits, Player& owner) : Actor(physic_world), m_owner(owner)
{
	m_moveSpeed = 100.0f;
	m_velocity = velocity;
	m_contacting = false;
	m_lifeTime = 5.0;
	m_life_timer.restart();

	/* --- Texture settings --- */

	m_Texture.loadFromFile("Assets/red_arrow.png");
	m_Texture.setSmooth(true);
	sf::Vector2u texture_size = m_Texture.getSize();
	m_size = b2Vec2(texture_size.x, texture_size.y);
	m_Sprite = sf::Sprite(m_Texture, sf::IntRect(0, 0, texture_size.x, texture_size.y));
	m_Sprite.setOrigin(texture_size.x / 2, texture_size.y / 2);

	/* --- Physics settings --- */

	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(position.x / 30, position.y / 30);

	body = physic_world.CreateBody(&bodyDef);
	m_Sprite.rotate(90 - atan2(velocity.x, velocity.y) * 180 / b2_pi);

	dynamicBox.SetAsBox(texture_size.x / 60.0f, texture_size.y / 60.0f);
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.0f;
	bodyDef.fixedRotation = true;
	fixtureDef.isSensor = true;
	fixtureDef.filter.categoryBits = categoryBits;
	fixtureDef.filter.maskBits = maskBits;
	body->CreateFixture(&fixtureDef);
	body->SetAngularVelocity(90 - atan2(velocity.x, velocity.y) * 180 / b2_pi);
	body->SetUserData(this); // On dit au body que c'est cette instance le owner pour les collisions apr�s par ex ... */

							 /* ------------------------- */

	body->SetLinearVelocity(b2Vec2(velocity.x * m_moveSpeed, velocity.y * m_moveSpeed));
}

void LetalShoot::update()
{
	Actor::update();

	/* On check si il est toujours en vie, sinon on le delete */
	if (!m_isAlive || m_life_timer.getElapsedTime().asSeconds() >= m_lifeTime)
	{
		destroy();
	}
}

void LetalShoot::startContact(Actor *player)
{
	m_isAlive = false;
}

void LetalShoot::destroy()
{

	auto it = std::find_if(m_owner.getLetalList().begin()
		, m_owner.getLetalList().end()
		, [this](auto & lookup)
	{
		return lookup.get() == this;
	});

	if (it != m_owner.getLetalList().end())
	{
		if (!m_physic_world.IsLocked())
		{
			m_physic_world.DestroyBody(body);
			body->SetUserData(nullptr);
			body = nullptr;
			m_owner.getLetalList().erase(it);
		}
	}
}
