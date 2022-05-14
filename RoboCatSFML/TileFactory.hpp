#pragma once

class TileFactory
{
protected:
	sf::Vector2u m_tile_size;

public:
	TileFactory(sf::Vector2u tile_size);
	virtual ~TileFactory() = default;

	virtual PlatformPart* CreatePlatformPart(ETileType tile_type, Vector3 spawn_pos, Platform* parent) const = 0;
	virtual Tile* CreateTile(ETileType tile_type, Vector3 spawn_pos, bool has_collider) const = 0;
	virtual sf::IntRect GetSubRect(ETileType tile_type, Vector3& spawn_pos) const;

	static sf::IntRect AdjustSubRect(sf::IntRect& sub_rect, ETileType tile_type, Vector3& spawn_pos);
};
