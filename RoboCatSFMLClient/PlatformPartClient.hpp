#pragma once
#include <SFML/Graphics/Rect.hpp>

class PlatformPartClient : public PlatformPart
{
private:
	SpriteComponentPtr m_sprite_component;

public:
	PlatformPartClient(sf::IntRect sub_rect, PlatformClient* platform, ETileType type);
	void SetTexture(TexturePtr texture, sf::IntRect sub_rect) const;
	sf::FloatRect GetBoundingRect() const override;
};
