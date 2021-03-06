//Written by Paul Bichler (D00242563)
#include "ColorJumpClientPCH.hpp"

//Written by Paul Bichler (D00242563)
//The Tile class is the base class of all tiles in the level
TileClient::TileClient(const sf::IntRect sub_rect, const ETileType type)
	: Tile(sub_rect, type)
{
	m_SpriteComponent.reset(new SpriteComponent(this));
	m_SpriteComponent->SetTexture(TextureManager::sInstance->GetTexture("LevelTileSet"), sub_rect);
}

//Written by Paul Bichler (D00242563)
sf::FloatRect TileClient::GetBoundingRect() const
{
	if (!m_has_collider)
		return GameObject::GetBoundingRect();

	return m_SpriteComponent->getTransform().transformRect(m_SpriteComponent->GetSprite().getLocalBounds());
}

sf::IntRect TileClient::GetSize() const
{
	return m_size;
}

void TileClient::SetSpriteTexture(TexturePtr texture, sf::IntRect subRect) const
{
	m_SpriteComponent->SetTexture(std::move(texture), subRect);
}
