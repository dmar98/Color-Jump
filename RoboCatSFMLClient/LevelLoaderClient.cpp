#include "RoboCatClientPCH.hpp"
#include "LevelLoaderClient.hpp"

LevelInfo LevelLoaderClient::LoadLevel(LevelManager::LevelData& level_data)
{
	m_tile_factory = std::make_unique<TileFactoryClient>(level_data.m_tile_size);
	return LevelLoader::LoadLevel(level_data);
}

void LevelLoaderClient::CreateTile(ETileType tile_type, Vector3 spawn_pos, bool has_collider)
{
	const GameObjectPtr tilePtr(m_tile_factory->CreateTile(tile_type, spawn_pos, has_collider));
	if (tilePtr.get() != nullptr)
		World::sInstance->AddGameObject(tilePtr);
}

void LevelLoaderClient::CreatePlatformPart(ETileType tile_type, Vector3 spawn_pos, Platform* platform)
{
	const std::shared_ptr<PlatformPart> platform_part(m_tile_factory->CreatePlatformPart(tile_type, spawn_pos, platform));
	World::sInstance->AddGameObject(platform_part);
}
