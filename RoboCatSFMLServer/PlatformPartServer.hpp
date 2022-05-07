#pragma once
#include <SFML/Graphics/Rect.hpp>

class PlatformPartServer : public PlatformPart
{
public:
	PlatformPartServer(sf::IntRect sub_rect, Platform* platform, ETileType type);
	sf::FloatRect GetBoundingRect() const override;
};
