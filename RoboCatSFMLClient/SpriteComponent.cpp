#include "RoboCatClientPCH.hpp"


SpriteComponent::SpriteComponent(GameObject* inGameObject)
	: DrawableComponent(inGameObject)
{
}

void SpriteComponent::SetTexture(const TexturePtr& inTexture, const sf::IntRect subRect)
{
	if(subRect != sf::IntRect(0,0,0,0))
		m_sprite.setTextureRect(subRect);

	m_sprite.setTexture(*inTexture);
	Utility::CentreOrigin(*this);
	m_sprite.setScale(sf::Vector2f(m_game_object->GetScale(), m_game_object->GetScale()));
}

sf::Sprite& SpriteComponent::GetSprite()
{
	return m_sprite;
}

void SpriteComponent::DrawCurrent(sf::RenderTarget& target, const sf::RenderStates states) const
{
	target.draw(m_sprite, states);
	const sf::FloatRect bounding_rect = m_game_object->GetBoundingRect();
	DrawBoundingRect(target, states, bounding_rect);
}

void SpriteComponent::DrawBoundingRect(sf::RenderTarget& target, sf::RenderStates states,
	const sf::FloatRect& bounding_rect)
{
	sf::RectangleShape shape;
	shape.setPosition(sf::Vector2f(bounding_rect.left, bounding_rect.top));
	shape.setSize(sf::Vector2f(bounding_rect.width, bounding_rect.height));
	shape.setFillColor(sf::Color::Transparent);
	shape.setOutlineColor(sf::Color::Green);
	shape.setOutlineThickness(1.f);
	target.draw(shape);
}

