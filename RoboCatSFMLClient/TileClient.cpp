//Written by Paul Bichler (D00242563)
#include "RoboCatClientPCH.hpp"

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
	/*if (!m_has_collider)
		return SpriteNode::GetBoundingRect();

	return GetWorldTransform().transformRect(m_sprite.getGlobalBounds());*/
	return sf::FloatRect();
}

bool TileClient::HandleCollision(const EColorType color)
{
	switch (m_type)
	{
	case kHorizontalBluePlatformPart:
	case kVerticalBluePlatformPart:
		if (color != EColorType::kBlue)
			return false;
		break;
	case kHorizontalRedPlatformPart:
	case kVerticalRedPlatformPart:
		if (color != EColorType::kRed)
			return false;
		break;
	default:
		break;
	}

	return true;
}

sf::IntRect TileClient::GetSize() const
{
	return m_size;
}

void TileClient::SetSpriteTexture(TexturePtr texture, sf::IntRect subRect) const
{
	m_SpriteComponent->SetTexture(std::move(texture), subRect);
}
