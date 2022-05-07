#pragma once

class TileFactoryClient : public TileFactory
{
private:
	unsigned int m_tile_map_columns{};

public:
	TileFactoryClient(sf::Vector2u tile_size);

	PlatformPart* CreatePlatformPart(ETileType tile_type, Vector3 spawn_pos, Platform* parent) const override;
	Tile* CreateTile(ETileType tile_type, Vector3 spawn_pos, bool has_collider) const override;
	sf::IntRect GetSubRect(ETileType tile_type, Vector3& spawn_pos) const override;
};
