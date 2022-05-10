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

Character* WorldServer::AddCharacterWithColor(const int player_id, const EColorType color,
                                              const sf::IntRect rect, const Vector3 spawn_pos)
{
	const auto player = new CharacterServer(color, rect);
	const GameObjectPtr player_ptr = NetworkManagerServer::sInstance->RegisterAndReturn(player);

	player->SetLocation(spawn_pos);
	player->SetIdentifier(player_id);

	m_players.emplace_back(player);
	AddGameObject(player_ptr);
	return player;
}

Character* WorldServer::AddCharacter(const int identifier, const EColorType color)
{
	return AddCharacterWithColor(identifier, color, m_level_info.m_blue_player_rect,
	                             m_level_info.m_blue_player_spawn_pos);
}

Character* WorldServer::AddGhostCharacterWithColor(const int identifier, EColorType color,
                                                   const sf::IntRect& int_rect,
                                                   const Vector3 spawn_pos)
{
	auto ghost_char = new CharacterServer(color, int_rect);
	const GameObjectPtr ghost_ptr = NetworkManagerServer::sInstance->RegisterAndReturn(ghost_char);
	ghost_char->SetLocation(spawn_pos);
	ghost_char->SetIdentifier(identifier);

	m_players.emplace_back(ghost_char);
	AddGameObject(ghost_ptr);
	return ghost_char;
}

Character* WorldServer::AddGhostCharacter(const int identifier, const EColorType color)
{
	return AddGhostCharacterWithColor(identifier, color,
	                                  m_level_info.m_blue_player_rect,
	                                  m_level_info.m_blue_player_spawn_pos);
}

void WorldServer::HandleCollisions()
{
	World::HandleCollisions();
}
