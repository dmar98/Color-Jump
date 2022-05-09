#include "RoboCatClientPCH.hpp"


SpriteComponent::SpriteComponent(GameObject* inGameObject)
	: DrawableComponent(inGameObject)
{
}

void SpriteComponent::SetTexture(TexturePtr inTexture, sf::IntRect subRect)
{
	auto tSize = inTexture->getSize();

	if(subRect != sf::IntRect(0,0,0,0))
		m_sprite.setTextureRect(subRect);

	m_sprite.setTexture(*inTexture);
	m_sprite.setOrigin(tSize.x / 2, tSize.y / 2);
	m_sprite.setScale(sf::Vector2f(1.f * m_game_object->GetScale(), 1.f * m_game_object->GetScale()));
}

sf::Sprite& SpriteComponent::GetSprite()
{
	return m_sprite;
}

void SpriteComponent::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_sprite, states);
}

