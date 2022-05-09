#pragma once
#include <SFML/Graphics/Rect.hpp>

class TileServer : public Tile
{
public:
	static GameObjectPtr StaticCreate(sf::IntRect sub_rect, ETileType type)
	{
		return NetworkManagerServer::sInstance->RegisterAndReturn(new TileServer(sub_rect, type));
	}

	TileServer(sf::IntRect sub_rect, ETileType type);
};

