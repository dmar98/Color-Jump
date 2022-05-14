#pragma once

class PlatformPart : public Tile
{
public:
	PlatformPart(sf::IntRect sub_rect, Platform* platform, ETileType type);
	Platform* GetPlatform() const;
	unsigned GetCategory() const override;

protected:
	Platform* m_parent;
};
