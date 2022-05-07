#include "RoboCatServerPCH.hpp"

LevelLoaderServer::LevelLoaderServer(LevelManager::LevelData& level_data)
	: LevelLoader(level_data)
{
	m_tile_factory = std::make_unique<TileFactoryServer>(level_data.m_tile_size);
}

void LevelLoaderServer::CreateTile(ETileType tile_type, Vector3 spawn_pos, bool has_collider)
{
	const GameObjectPtr tilePtr = NetworkManagerServer::sInstance->RegisterAndReturn(m_tile_factory->CreateTile(tile_type, spawn_pos, has_collider));
	if (tilePtr.get() != nullptr)
		World::sInstance->AddGameObject(tilePtr);
}

void LevelLoaderServer::CreatePlatformPart(ETileType tile_type, Vector3 spawn_pos, Platform* platform)
{
	const GameObjectPtr platform_part = NetworkManagerServer::sInstance->RegisterAndReturn(m_tile_factory->CreatePlatformPart(tile_type, spawn_pos, platform));
	World::sInstance->AddGameObject(platform_part);
}
