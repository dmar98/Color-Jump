#include "RoboCatPCH.hpp"

std::unique_ptr<World> World::sInstance;

void World::StaticInit(World* world_instance)
{
	sInstance.reset(world_instance);
}

World::World()
{
}

void World::HandleCollisions()
{

}


void World::AddGameObject(GameObjectPtr inGameObject)
{
	mGameObjects.push_back(inGameObject);
	inGameObject->SetIndexInWorld(mGameObjects.size() - 1);
}


void World::RemoveGameObject(GameObjectPtr inGameObject)
{
	int index = inGameObject->GetIndexInWorld();

	int lastIndex = mGameObjects.size() - 1;
	if (index != lastIndex)
	{
		mGameObjects[index] = mGameObjects[lastIndex];
		mGameObjects[index]->SetIndexInWorld(index);
	}

	inGameObject->SetIndexInWorld(-1);

	mGameObjects.pop_back();
}


void World::Update()
{
	for (const auto& platform : m_level_info.platforms)
	{
		platform->Update();
	}

	//update all game objects- sometimes they want to die, so we need to tread carefully...
	for (int i = 0, c = mGameObjects.size(); i < c; ++i)
	{
		const GameObjectPtr go = mGameObjects[i];


		if (!go->DoesWantToDie())
		{
			go->Update();
		}

		//you might suddenly want to die after your update, so check again
		if (go->DoesWantToDie())
		{
			RemoveGameObject(go);
			go->HandleDying();
			--i;
			--c;
		}
	}
}

void World::RemoveAllGameObjects()
{
	mGameObjects.clear();
}

void World::SetWorldBounds(sf::FloatRect world_bounds)
{
	m_world_bounds = world_bounds;
}

sf::FloatRect& World::GetWorldBounds()
{
	return m_world_bounds;
}
