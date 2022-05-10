#include "RoboCatServerPCH.hpp"

WorldServer::WorldServer() : World()
{
}

WorldServer::~WorldServer() = default;

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

Character* WorldServer::AddCharacter(const int identifier, const EColorType color)
{
	const auto player = new CharacterServer(color, m_level_info.m_blue_player_rect);
	const GameObjectPtr player_ptr = NetworkManagerServer::sInstance->RegisterAndReturn(player);

	player->SetLocation(m_level_info.m_blue_player_spawn_pos);
	player->SetIdentifier(identifier);

	m_players.emplace_back(player);
	AddGameObject(player_ptr);
	return player;
}

Character* WorldServer::AddGhostCharacter(const int identifier, const EColorType color)
{
	auto ghost_char = new CharacterServer(color, m_level_info.m_blue_player_rect);
	const GameObjectPtr ghost_ptr = NetworkManagerServer::sInstance->RegisterAndReturn(ghost_char);
	ghost_char->SetLocation(m_level_info.m_blue_player_spawn_pos);
	ghost_char->SetIdentifier(identifier);

	m_players.emplace_back(ghost_char);
	AddGameObject(ghost_ptr);
	return ghost_char;
}

void WorldServer::HandleCollisions()
{
	World::HandleCollisions();
}
