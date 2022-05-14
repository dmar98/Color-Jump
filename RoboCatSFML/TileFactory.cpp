//Written by Paul Bichler (D00242563)
#include "ColorJumpPCH.hpp"

TileFactory::TileFactory(sf::Vector2u tile_size)
	: m_tile_size(tile_size)
{
}

//Written by Paul Bichler (D00242563)
//Gets the sub rect of the tile type based on X and Y coordinates of the type value.
//The method also adjusts the sub rect to adjust the collider.
sf::IntRect TileFactory::GetSubRect(const ETileType tile_type, Vector3& spawn_pos) const
{
	sf::IntRect sub_rect;
	sub_rect.height = m_tile_size.y;
	sub_rect.width = m_tile_size.x;

	//adjust rect height and width based on the tile (to correct the collider)
	return AdjustSubRect(sub_rect, tile_type, spawn_pos);
}

//Written by Paul Bichler (D00242563)
//Adjusts the rect height and width based on the tile (to get rid of the outer transparent pixels of the tile)
sf::IntRect TileFactory::AdjustSubRect(sf::IntRect& sub_rect, ETileType tile_type, Vector3& spawn_pos)
{
	switch (tile_type)
	{
	case kHorizontalImpactPlatformPart:
	case kHorizontalBluePlatformPart:
	case kHorizontalRedPlatformPart:
		sub_rect.height = 23;
		break;
	case kVerticalImpactPlatformPart:
	case kVerticalBluePlatformPart:
	case kVerticalRedPlatformPart:
		sub_rect.width = 20;
		sub_rect.left += 22;
		spawn_pos.mX += 22;
		break;
	case kBluePlayer:
	case kRedPlayer:
		sub_rect.height = 61;
		sub_rect.left += 12;
		spawn_pos.mX += 32;
		sub_rect.width = 40;
		break;
	case kSpikes:
		sub_rect.top += 34;
		sub_rect.left += 4;
		sub_rect.height = 30;
		sub_rect.width = 56;
		spawn_pos.mY += 34;
		spawn_pos.mY += 4;
		break;
	}

	return sub_rect;
}
