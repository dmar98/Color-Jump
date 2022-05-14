//Written by Paul Bichler (D00242563)

#include "ColorJumpServerPCH.hpp"

//Written by Paul Bichler (D00242563)
//The Tile class is the base class of all tiles in the level
TileServer::TileServer(const sf::IntRect sub_rect, const ETileType type)
	: Tile(sub_rect, type)
{
}
