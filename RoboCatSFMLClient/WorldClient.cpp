#include "RoboCatClientPCH.hpp"

#include "LevelLoaderClient.hpp"

WorldClient::WorldClient()
	: World(),
	  m_checkpoint(nullptr)
{
}

void WorldClient::LoadLevel()
{
	LevelManager::sInstance->SetIsMultiplayer(true);
	LevelManager::LevelData level_data = LevelManager::sInstance->GetCurrentLevelData();

	LevelLoaderClient loader {};
	m_level_info = loader.LoadLevel(level_data);
}

void WorldClient::Update()
{
	World::Update();
}

void WorldClient::SetCamera(sf::View camera)
{
	SetWorldBounds(sf::FloatRect(0.f, 0.f, camera.getSize().x, camera.getSize().y));
	camera.setCenter(camera.getSize().x / 2.f, m_world_bounds.height - camera.getSize().y / 2.f);
	m_camera = camera;
}

sf::View& WorldClient::GetCamera()
{
	return m_camera;
}

void WorldClient::HandleCollisions()
{
	//std::set<SceneNode::Pair> collision_pairs;
	//m_sceneGraph.CheckSceneCollision(m_sceneGraph, collision_pairs, [this](SceneNode& node)
	//	{
	//		const auto character = dynamic_cast<Character*>(&node);
	//		const bool character_cond = character != nullptr;

	//		const auto ray_ground = dynamic_cast<RayGround*>(&node);
	//		const bool ray_cond = ray_ground != nullptr;

	//		//check collisions only for players and RayGround objects
	//		return character_cond || ray_cond;
	//	});

	//std::set<SceneNode::Pair> player_pair;

	//for (const SceneNode::Pair& pair : collision_pairs)
	//{
	//	if (CollisionHandler::PlatformCollision(dt, pair, [this] { OnReachedCheckpoint(); }, [this] { OnReachedGoal(); }, this))
	//		continue;

	//	CollisionHandler::TrapCollision(pair, [this] { OnClientPlayerDeath(); });

	//	//Get All Ground Ray Casts for player one and two
	//	CollisionHandler::GetGroundRayCasts(player_pair, pair, Category::kRay);
	//}

	//CollisionHandler::PlayerGroundRayCast(player_pair);
}
