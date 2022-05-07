#pragma once
#include <SFML/Graphics/Rect.hpp>

#include "Platform.hpp"
#include "Tile.hpp"

class PlatformPart : public Tile
{
public:
	PlatformPart(sf::IntRect sub_rect, Platform* platform, ETileType type);
	Platform* GetPlatform() const;

protected:
	Platform* m_parent;
};
