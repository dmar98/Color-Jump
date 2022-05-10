#include "RoboCatClientPCH.hpp"
#include "LevelLoaderClient.hpp"

LevelInfo LevelLoaderClient::LoadLevel(LevelManager::LevelData& level_data)
{
	m_tile_factory = std::make_unique<TileFactoryClient>(level_data.m_tile_size);
	return LevelLoader::LoadLevel(level_data);
}

void LevelLoaderClient::CreatePlatform(LevelInfo& level_info, ETileType tile_type, int row, int col, Vector3 spawn_pos,
	sf::Int8 platform_id)
{
	std::unique_ptr<PlatformClient> platform(new PlatformClient(platform_id, static_cast<EPlatformType>(tile_type)));
	AddPlatformParts(platform.get(), row, col, tile_type, spawn_pos);
	level_info.platforms.emplace_back(std::move(platform));
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
