//Written by Paul Bichler (D00242563)
#include "RoboCatPCH.hpp"

//Written by Paul Bichler (D00242563)
//The Tile class is the base class of all tiles in the level
Tile::Tile(const sf::IntRect sub_rect, const ETileType type)
	: GameObject(),
	  m_type(type),
	  m_has_collider(false),
	  m_size(sub_rect)
{
}

//Written by Paul Bichler (D00242563)
ETileType Tile::GetType() const
{
	return m_type;
}

//Written by Paul Bichler (D00242563)
//Not all tiles need collision detection (tiles on the background layer for example)
void Tile::SetActiveCollider(bool active)
{
	m_has_collider = active;
}

//Written by Paul Bichler (D00242563)
sf::FloatRect Tile::GetBoundingRect() const
{
	/*if (!m_has_collider)
		return SpriteNode::GetBoundingRect();

	return GetWorldTransform().transformRect(m_sprite.getGlobalBounds());*/
	return sf::FloatRect();
}

bool Tile::HandleCollision(const EColorType color)
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

sf::IntRect Tile::GetSize() const
{
	return m_size;
}
