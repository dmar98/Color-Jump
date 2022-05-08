#include "LevelLoader.hpp"
/*
* the world tracks all the live game objects. Failry inefficient for now, but not that much of a problem
*/
class World
{

public:

	static void StaticInit();
	static std::unique_ptr<World> sInstance;

	void AddGameObject(GameObjectPtr inGameObject);
	void RemoveGameObject(GameObjectPtr inGameObject);
	void Update();
	void LoadLevel();
	void DisposeLevel();

	const std::vector<GameObjectPtr>& GetGameObjects() const { return mGameObjects; }

protected:

	World();

	int	GetIndexOfGameObject(GameObjectPtr inGameObject);

	std::vector<GameObjectPtr> mGameObjects;
	std::unique_ptr<LevelLoader> mLevelLoader;
};

