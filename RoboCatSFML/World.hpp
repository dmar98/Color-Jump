#pragma once

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
	void RemoveGameObject(const GameObjectPtr& inGameObject);
	void RemoveAllGameObjects();

	void SetWorldBounds(sf::FloatRect world_bounds);
	sf::FloatRect& GetWorldBounds();
	const std::vector<GameObjectPtr>& GetGameObjects() const { return mGameObjects; }

	virtual void Update();
	virtual void LoadLevel() = 0;

	virtual Character* AddCharacter(int identifier, EColorType color) = 0;
	virtual Character* AddGhostCharacter(const int identifier, const EColorType color) = 0;

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

