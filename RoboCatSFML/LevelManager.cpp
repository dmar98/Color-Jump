#include "RoboCatPCH.hpp"
//Written by Paul Bichler (D00242563)

std::unique_ptr<LevelManager> LevelManager::sInstance;

void LevelManager::StaticInit()
{
	sInstance.reset(new LevelManager());
}

//Written by Paul Bichler (D00242563)
//This struct holds all the necessary level data info to load a level
LevelManager::LevelData::LevelData(std::string platform_layer_path, std::string background_layer_path, const sf::Vector2u tile_size)
	: m_platform_layer_path(std::move(platform_layer_path)),
	m_background_layer_path(std::move(background_layer_path)),
	m_tile_size(tile_size)
{
}

//Written by Paul Bichler (D00242563)
LevelManager::LevelManager() : m_current_coop_index(0), m_current_multiplayer_index(0)
{
	//Holds level data info for coop levels (offline)
	m_coop_levels.emplace_back("../Assets/LevelData/Coop/LevelData_Level1_Platforms.csv", "../Assets/LevelData/Coop/LevelData_Level1_Background.csv", sf::Vector2u(64, 64));
	m_coop_levels.emplace_back("../Assets/LevelData/Coop/LevelData_Level2_Platforms.csv", "../Assets/LevelData/Coop/LevelData_Level2_Background.csv", sf::Vector2u(64, 64));
	m_coop_levels.emplace_back("../Assets/LevelData/Coop/LevelData_Level3_Platforms.csv", "../Assets/LevelData/Coop/LevelData_Level3_Background.csv", sf::Vector2u(64, 64));
	m_coop_levels.emplace_back("../Assets/LevelData/Coop/LevelData_Level4_Platforms.csv", "../Assets/LevelData/Coop/LevelData_Level4_Background.csv", sf::Vector2u(64, 64));
	m_coop_levels.emplace_back("../Assets/LevelData/Coop/LevelData_Level5_Platforms.csv", "../Assets/LevelData/Coop/LevelData_Level5_Background.csv", sf::Vector2u(64, 64));
	m_coop_levels.emplace_back("../Assets/LevelData/Coop/LevelData_Level6_Platforms.csv", "../Assets/LevelData/Coop/LevelData_Level6_Background.csv", sf::Vector2u(64, 64));
	m_coop_levels.emplace_back("../Assets/LevelData/Coop/LevelData_Level7_Platforms.csv", "../Assets/LevelData/Coop/LevelData_Level7_Background.csv", sf::Vector2u(64, 64));
	m_coop_levels.emplace_back("../Assets/LevelData/Coop/LevelData_Level8_Platforms.csv", "../Assets/LevelData/Coop/LevelData_Level8_Background.csv", sf::Vector2u(64, 64));
	m_coop_levels.emplace_back("../Assets/LevelData/Coop/LevelData_Level9_Platforms.csv", "../Assets/LevelData/Coop/LevelData_Level9_Background.csv", sf::Vector2u(64, 64));

	//Holds level data info for multiplayer levels (online)
	m_multiplayer_levels.emplace_back("../Assets/LevelData/Multiplayer/LevelData_Multiplayer_Level1_ColliderTiles.csv", "../Assets/LevelData/Multiplayer/LevelData_Multiplayer_Level1_BackgroundTiles.csv", sf::Vector2u(64, 64));
	//m_multiplayer_levels.emplace_back("../Assets/LevelData/Multiplayer/Multiplayer_Test_ColliderTiles.csv", "../Assets/LevelData/Multiplayer/Multiplayer_Test_BackgroundTiles.csv", sf::Vector2u(64, 64));
}

//Written by Paul Bichler (D00242563)
LevelManager::LevelData LevelManager::GetCurrentLevelData() const
{
	if (m_is_multiplayer)
		return m_multiplayer_levels[m_current_multiplayer_index];

	return m_coop_levels[m_current_coop_index];
}

//Written by Paul Bichler (D00242563)
void LevelManager::NextLevel()
{
	if(DoesNextLevelExist())
		m_current_coop_index++;
}

//Written by Paul Bichler (D00242563)
bool LevelManager::DoesNextLevelExist() const
{
	return m_current_coop_index + 1 < m_coop_levels.size();
}

//Written by Paul Bichler (D00242563)
void LevelManager::SetIsMultiplayer(bool is_multiplayer)
{
	m_is_multiplayer = is_multiplayer;
}
