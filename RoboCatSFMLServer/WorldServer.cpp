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

Character* WorldServer::AddCharacterWithColor(sf::Int8 identifier, EColorType color, sf::IntRect rect, Vector3 spawn_pos)
{
	const auto player = new CharacterServer(color, rect);
	const GameObjectPtr player_ptr = NetworkManagerServer::sInstance->RegisterAndReturn(player);

	player->SetLocation(spawn_pos);
	player->SetIdentifier(identifier);

	m_players.emplace_back(player);
	AddGameObject(player_ptr);
	return player;
}

Character* WorldServer::AddCharacter(int identifier, int color, bool is_client_player)
{
	if (color == 1)
	{
		return AddCharacterWithColor(identifier, EColorType::kRed, m_level_info.m_red_player_rect,
			m_level_info.m_red_player_spawn_pos);
	}

	return AddCharacterWithColor(identifier, EColorType::kBlue, m_level_info.m_blue_player_rect,
	                             m_level_info.m_blue_player_spawn_pos);

}

Character* WorldServer::AddGhostCharacterWithColor(const int identifier, EColorType color, const sf::IntRect& int_rect,
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

Character* WorldServer::AddGhostCharacter(int identifier, int color)
{
	if (color == 1)
	{
		return AddGhostCharacterWithColor(identifier, EColorType::kRed,
		                                  m_level_info.m_red_player_rect,
		                                  m_level_info.m_red_player_spawn_pos);
	}

	return AddGhostCharacterWithColor(identifier, EColorType::kBlue,
	                                  m_level_info.m_blue_player_rect,
	                                  m_level_info.m_blue_player_spawn_pos);
}
