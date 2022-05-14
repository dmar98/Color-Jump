//Written by Paul Bichler (D00242563)
#include "ColorJumpPCH.hpp"

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
//Returns the right category based on the tile type
unsigned Tile::GetCategory() const
{
	//Return the default category for non-collider tiles
	if (!m_has_collider)
		return Category::kPlatform;

	switch (m_type)
	{
	case kSpikes:
		return Category::kTrap;
	default:
		return Category::kPlatform;
	}
}

bool Tile::HandleCollision(const EColorType color)
{
	switch (m_type)
	{
	case kHorizontalBluePlatformPart:
	case kVerticalBluePlatformPart:
		if (color != kBlue)
			return false;
		break;
	case kHorizontalRedPlatformPart:
	case kVerticalRedPlatformPart:
		if (color != kRed)
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
