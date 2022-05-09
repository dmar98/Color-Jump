#pragma once
#include "ETileType.hpp"


class TileClient : public Tile
{
public:
	static GameObjectPtr StaticCreate(sf::IntRect sub_rect, ETileType type)
	{
		return std::make_shared<TileClient>(sub_rect, type);
	}

	TileClient(sf::IntRect sub_rect, ETileType type);

	sf::FloatRect GetBoundingRect() const override;
	sf::IntRect GetSize() const override;
	void SetSpriteTexture(TexturePtr texture, sf::IntRect subRect) const;

protected:
	SpriteComponentPtr	m_SpriteComponent;
};

