#pragma once
#include <SFML/Graphics/Rect.hpp>

#include "LevelManager.hpp"
#include "TileFactory.hpp"

#include <memory>

struct LevelInfo
{
	sf::IntRect m_blue_player_rect;
	Vector3 m_blue_player_spawn_pos;
	sf::IntRect m_red_player_rect;
	Vector3 m_red_player_spawn_pos;

	std::vector<std::unique_ptr<Platform>> platforms;

	Platform* GetPlatformWithID(const sf::Int8 platform_id) const
	{
		for (const auto& platform : platforms)
			if (platform->GetID() == platform_id)
				return platform.get();

		return nullptr;
	}
};

class LevelLoader
{
public:
	virtual ~LevelLoader() = default;
	LevelLoader(LevelManager::LevelData& level_data);
	LevelInfo LoadLevel();

private:
	virtual void LoadLevelLayer(const std::string& csv_path, LevelInfo& level_info, bool is_collider_layer);
	std::vector<std::vector<int>> LevelDataToVector(const std::string& csv_path) const;
	void CreatePlatform(LevelInfo& level_info, ETileType tile_type, int row, int col, Vector3 spawn_pos, sf::Int8 platform_id);
	void AddPlatformParts(Platform* platform, int row, int col, ETileType tile_type, Vector3 spawn_pos);

protected:
	virtual void CreateTile(ETileType tile_type, Vector3 spawn_pos, bool has_collider) = 0;
	virtual void CreatePlatformPart(ETileType tile_type, Vector3 spawn_pos, Platform* platform) = 0;

protected:
	LevelManager::LevelData& m_level_data;
	std::unique_ptr<TileFactory> m_tile_factory;
	std::vector<std::vector<int>> m_level_data_vector;
};
