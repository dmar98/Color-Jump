//Written by Paul Bichler (D00242563)
#include "RoboCatPCH.hpp"

#include "LevelManager.hpp"

std::unique_ptr<LevelLoader> LevelLoader::sInstance;

void LevelLoader::StaticInit(LevelLoader* level_loader)
{
	sInstance.reset(level_loader);
}

//Written by Paul Bichler (D00242563)
//The LevelLoader class is used to construct levels based on the level data CSV files in the LevelManager.
//It uses the TileFactory class to create instances for the tile types specified in the level files.
LevelLoader::LevelLoader() = default;

//Written by Paul Bichler (D00242563)
//This method constructs the level from the CSV files in the level data member field.
//Return a LevelInfo struct that holds all of the information about the level.
LevelInfo LevelLoader::LoadLevel(LevelManager::LevelData& level_data)
{
	m_tile_size = level_data.m_tile_size;

	LevelInfo level_info;
	LoadLevelLayer(level_data.m_background_layer_path, level_info, false);
	LoadLevelLayer(level_data.m_platform_layer_path, level_info, true);

	return level_info;
}

//Written by Paul Bichler (D00242563)
//Construct a level layer from the file at the specified path
void LevelLoader::LoadLevelLayer(const std::string& csv_path, LevelInfo& level_info, const bool is_collider_layer)
{
	//Create the root node
	Vector3 spawn_pos(0.f, 0.f, 0.f);

	//Convert the csv level data into a 2-dimensional vector (this makes it easier to create platforms)
	m_level_data_vector = LevelDataToVector(csv_path);

	sf::Int8 platform_id = 0;

	//Loop through the 2-dimensional level vector and construct the level tile by tile
	for (int row = 0; row < m_level_data_vector.size(); row++)
	{
		for (int col = 0; col < m_level_data_vector[row].size(); col++)
		{
			//The id in the level data CSV is converted to a TileType enum to make the code more readable and easily adjustable
			int id = m_level_data_vector[row][col];
			const auto tile_type = static_cast<ETileType>(id);

			//Construct the tile based on the tile type
			switch (tile_type)
			{
			case kRedPlayer:
			{
				//Do not swap these lines as for red the spawn position is set in GetSubRect
				level_info.m_red_player_rect = m_tile_factory->GetSubRect(kRedPlayer, spawn_pos);
				level_info.m_red_player_spawn_pos = spawn_pos;
			}
			break;
			case kBluePlayer:
			{
				//Do not swap these lines as for blue the spawn position is set before GetSubRect
				level_info.m_blue_player_spawn_pos = spawn_pos;
				level_info.m_blue_player_rect = m_tile_factory->GetSubRect(kBluePlayer, spawn_pos);
			}
			break;
			case kHorizontalPlatformPart:
			case kHorizontalImpactPlatformPart:
			case kVerticalImpactPlatformPart:
			case kHorizontalPulsePlatformPart:
			case kVerticalPulsePlatformPart:
			case kHorizontalBluePlatformPart:
			case kHorizontalRedPlatformPart:
			case kVerticalBluePlatformPart:
			case kVerticalRedPlatformPart:
			case kFinishPlatformPart:
			case kCheckpointPlatformPart:
				//Construct a platform (the platform type is determined by the tile type)
				{
					//Construct a platform (the platform type is determined by the tile type)
					CreatePlatform(level_info, tile_type, row, col, spawn_pos, platform_id++);
				}
			break;
			case kNone:
				//Tiles marked with (-1 = kNone) do nothing
				break;
			default:
			{
				//Tiles that don't correspond to one of the types above have no special functionality,
				//which is why they are created here with the base "Tile" class.
				CreateTile(tile_type, spawn_pos, is_collider_layer);
			}
			}

			spawn_pos.mX += m_tile_size.x;
		}

		spawn_pos.mX = 0.f;
		spawn_pos.mY += m_tile_size.y;
	}
}

//Written by Paul Bichler (D00242563)
//This method reads a csv level file line-by-line and converts it to a 2-dimensional vector
std::vector<std::vector<int>> LevelLoader::LevelDataToVector(const std::string& csv_path) const
{
	std::vector<std::vector<int>> levelDataVector;
	std::ifstream in(csv_path);
	assert(!in.fail());

	std::string line;
	while (std::getline(in, line))
	{
		std::vector<int> levelLineData;

		std::stringstream ss(line);
		std::string token;

		while (std::getline(ss, token, ','))
			levelLineData.emplace_back(std::stoi(token));

		levelDataVector.emplace_back(levelLineData);
	}

	return levelDataVector;
}

//Written by Paul Bichler (D00242563)
//This method is called when a platform part is detected and searches for all the other
//platform parts connected to it, to construct a platform.
void LevelLoader::CreatePlatform(LevelInfo& level_info, const ETileType tile_type,
	const int row, const int col, const Vector3 spawn_pos, const sf::Int8 platform_id)
{
	std::unique_ptr<Platform> platform(new Platform(platform_id, static_cast<EPlatformType>(tile_type)));
	AddPlatformParts(platform.get(), row, col, tile_type, spawn_pos);
	level_info.platforms.emplace_back(std::move(platform));
}

//Written by Paul Bichler (D00242563)
//This method is used to add platform parts to a platform. It loops until no platform part is detected.
void LevelLoader::AddPlatformParts(Platform* platform, const int row, const int col,const ETileType tile_type, Vector3 spawn_pos)
{
	const auto type = static_cast<ETileType>(m_level_data_vector[row][col]);

	CreatePlatformPart(tile_type, spawn_pos, platform);

	//Is vertical platform
	for (size_t i = row + 1; i < m_level_data_vector.size(); i++)
	{
		//Stop the loop when the tile doesn't belong to the platform
		if (static_cast<ETileType>(m_level_data_vector[i][col]) != type)
			break;

		spawn_pos.mY += m_tile_size.y;
		CreatePlatformPart(tile_type, spawn_pos, platform);

		//Set the tile type to -1 to prevent duplication
		m_level_data_vector[i][col] = -1;
	}

	//Is horizontal platform
	for (size_t j = col + 1; j < m_level_data_vector[row].size(); j++)
	{
		//Stop the loop when the tile doesn't belong to the platform
		if (static_cast<ETileType>(m_level_data_vector[row][j]) != type)
			break;

		spawn_pos.mX += m_tile_size.x;
		CreatePlatformPart(tile_type, spawn_pos, platform);

		//Set the tile type to -1 to prevent duplication
		m_level_data_vector[row][j] = -1;
	}
}
