#include "RoboCatServerPCH.hpp"

WorldServer::WorldServer() : World()
{

}

void WorldServer::Update()
{
	World::Update();
}

void WorldServer::LoadLevel()
{
	LevelManager::sInstance->SetIsMultiplayer(true);
	LevelManager::LevelData level_data = LevelManager::sInstance->GetCurrentLevelData();

	LevelLoaderServer loader{};
	m_level_info = loader.LoadLevel(level_data);
}
