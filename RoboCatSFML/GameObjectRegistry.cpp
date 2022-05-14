#include "ColorJumpPCH.hpp"


std::unique_ptr< GameObjectRegistry >	GameObjectRegistry::sInstance;

void GameObjectRegistry::StaticInit()
{
	sInstance.reset(new GameObjectRegistry());
}

GameObjectRegistry::GameObjectRegistry() = default;

void GameObjectRegistry::RegisterCreationFunction(const uint32_t inFourCCName, const GameObjectCreationFunc inCreationFunction)
{
	mNameToGameObjectCreationFunctionMap[inFourCCName] = inCreationFunction;
}

GameObjectPtr GameObjectRegistry::CreateGameObject(const uint32_t inFourCCName)
{
	//no error checking- if the name isn't there, exception!
	const GameObjectCreationFunc creationFunc = mNameToGameObjectCreationFunctionMap[inFourCCName];

	GameObjectPtr gameObject = creationFunc();

	//should the registry depend on the world? this might be a little weird
	//perhaps you should ask the world to spawn things? for now it will be like this
	World::sInstance->AddGameObject(gameObject);

	return gameObject;
}