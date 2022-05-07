#pragma once

class TileFactoryServer : public TileFactory
{
public:
	TileFactoryServer(sf::Vector2u tile_size);
	PlatformPart* CreatePlatformPart(ETileType tile_type, Vector3 spawn_pos, Platform* parent) const override;
	Tile* CreateTile(ETileType tile_type, Vector3 spawn_pos, bool has_collider) const override;
};

