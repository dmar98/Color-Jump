#pragma once
class LevelLoaderServer : public LevelLoader
{
public:
	LevelLoaderServer();
	LevelInfo LoadLevel(LevelManager::LevelData& level_data) override;

protected:
	void CreateTile(ETileType tile_type, Vector3 spawn_pos, bool has_collider) override;
	void CreatePlatformPart(ETileType tile_type, Vector3 spawn_pos, Platform* platform) override;
};

