//Written by Paul Bichler (D00242563)
#include "RoboCatClientPCH.hpp"

//Written by Paul Bichler (D00242563)
TileFactoryClient::TileFactoryClient(const sf::Vector2u tile_size)
	: TileFactory(tile_size)
{
	m_tile_map_columns = TextureManager::sInstance->GetTexture("LevelTileSet")->getSize().x / tile_size.x;
}

//Written by Paul Bichler (D00242563)
//Gets the sub rect of the tile type based on X and Y coordinates of the type value.
//The method also adjusts the sub rect to adjust the collider.
sf::IntRect TileFactoryClient::GetSubRect(const ETileType tile_type, Vector3& spawn_pos) const
{
	//The same could be achieved by looping, this is however much simpler.
	//Got the answer from here: https://stackoverflow.com/questions/52825969/getting-x-and-y-coordinates-from-tile-id
	sf::IntRect sub_rect;
	sub_rect.top = m_tile_size.y * (tile_type / m_tile_map_columns);
	sub_rect.height = m_tile_size.y;
	sub_rect.left = m_tile_size.x * (tile_type % m_tile_map_columns);
	sub_rect.width = m_tile_size.x;

	//adjust rect height and width based on the tile (to correct the collider)
	return AdjustSubRect(sub_rect, tile_type, spawn_pos);
}

//Written by Paul Bichler (D00242563)
PlatformPart* TileFactoryClient::CreatePlatformPart(const ETileType tile_type, Vector3 spawn_pos, Platform* parent) const
{
	const sf::IntRect sub_rect = GetSubRect(tile_type, spawn_pos);

	const auto platform_part = new PlatformPartClient(sub_rect, dynamic_cast<PlatformClient*>(parent), tile_type);
	platform_part->SetLocation(spawn_pos);
	parent->AddPlatformPart(platform_part);

	return platform_part;
}

//Written by Paul Bichler (D00242563)
Tile* TileFactoryClient::CreateTile(const ETileType tile_type, Vector3 spawn_pos, const bool has_collider) const
{
	const sf::IntRect sub_rect = GetSubRect(tile_type, spawn_pos);

	const auto tile = new TileClient(sub_rect, tile_type);
	tile->SetActiveCollider(has_collider);
	tile->SetLocation(spawn_pos);

	return tile;
}
