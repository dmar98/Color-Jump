#include "RoboCatClientPCH.hpp"


SpriteComponent::SpriteComponent(GameObject* inGameObject)
	: DrawableComponent(inGameObject)
{
}

void SpriteComponent::SetTexture(TexturePtr inTexture, sf::IntRect subRect)
{
	if(subRect != sf::IntRect(0,0,0,0))
		m_sprite.setTextureRect(subRect);

	m_sprite.setTexture(*inTexture);
	Utility::CentreOrigin(m_sprite);
	m_sprite.setScale(sf::Vector2f(m_game_object->GetScale(), m_game_object->GetScale()));
}

sf::Sprite& SpriteComponent::GetSprite()
{
	return m_sprite;
}

void SpriteComponent::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_sprite, states);
}

