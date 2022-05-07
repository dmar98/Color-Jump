#include "RoboCatServerPCH.hpp"
#include "TileFactoryServer.hpp"

TileFactoryServer::TileFactoryServer(sf::Vector2u tile_size) : TileFactory(tile_size)
{
}

PlatformPart* TileFactoryServer::CreatePlatformPart(ETileType tile_type, Vector3 spawn_pos, Platform* parent) const
{
	const sf::IntRect sub_rect = GetSubRect(tile_type, spawn_pos);

	const auto platform_part = new PlatformPartServer(sub_rect, parent, tile_type);
	platform_part->SetLocation(spawn_pos);
	parent->AddPlatformPart(platform_part);

	return platform_part;
}

Tile* TileFactoryServer::CreateTile(ETileType tile_type, Vector3 spawn_pos, bool has_collider) const
{
	const sf::IntRect sub_rect = GetSubRect(tile_type, spawn_pos);

	const auto tile = new TileServer(sub_rect, tile_type);
	tile->SetActiveCollider(has_collider);
	tile->SetLocation(spawn_pos);

	return tile;
}
