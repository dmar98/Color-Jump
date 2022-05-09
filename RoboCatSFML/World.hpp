#pragma once
#include "Character.hpp"
#include "LevelLoader.hpp"

/*
* the world tracks all the live game objects. Fairly inefficient for now, but not that much of a problem
*/
class World
{
public:
	static void StaticInit(World* world_instance);
	virtual ~World() = default;
	static std::unique_ptr<World> sInstance;

	void AddGameObject(GameObjectPtr inGameObject);
	void RemoveGameObject(GameObjectPtr inGameObject);
	void RemoveAllGameObjects();

	void SetWorldBounds(sf::FloatRect world_bounds);
	sf::FloatRect& GetWorldBounds();
	const std::vector<GameObjectPtr>& GetGameObjects() const { return mGameObjects; }

	virtual void Update();
	virtual void LoadLevel() = 0;

	virtual Character* AddCharacterWithColor(sf::Int8 identifier, EColorType color, sf::IntRect rect, Vector3 spawn_pos) = 0;
	virtual Character* AddCharacter(sf::Int8 identifier, sf::Int8 color, bool is_client_player) = 0;
	virtual Character* AddGhostCharacterWithColor(sf::Int8 identifier, EColorType color, const sf::IntRect& int_rect, Vector3 spawn_pos) = 0;
	virtual Character* AddGhostCharacter(sf::Int8 identifier, sf::Int8 color) = 0;

protected:
	World();

	virtual void HandleCollisions();
	int	GetIndexOfGameObject(GameObjectPtr inGameObject);

	std::vector<GameObjectPtr> mGameObjects;
	std::unique_ptr<LevelLoader> mLevelLoader;

	sf::FloatRect m_world_bounds;
	LevelInfo m_level_info;
	std::vector<Character*> m_players;
};

