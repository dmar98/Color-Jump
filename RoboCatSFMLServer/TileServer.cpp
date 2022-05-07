//Written by Paul Bichler (D00242563)

#include "RoboCatServerPCH.hpp"

//Written by Paul Bichler (D00242563)
//The Tile class is the base class of all tiles in the level
TileServer::TileServer(const sf::IntRect sub_rect, const ETileType type)
	: Tile(sub_rect, type)
{
}


////Written by Paul Bichler (D00242563)
////Returns the right category based on the tile type
//unsigned TileClient::GetCategory() const
//{
//	//Return the default category for non-collider tiles
//	if (!m_has_collider)
//		return Category::kPlatform;
//
//	switch (m_type)
//	{
//	case kSpikes:
//		return Category::kEnemyTrap;
//	default:
//		return Category::kPlatform;
//	}
//}


bool TileServer::HandleCollision(const EColorType color)
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
