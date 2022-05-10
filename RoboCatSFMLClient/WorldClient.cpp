#include "RoboCatClientPCH.hpp"

WorldClient::WorldClient()
	: World(),
	  m_checkpoint(nullptr)
{
}

void WorldClient::LoadLevel()
{
	LevelManager::sInstance->SetIsMultiplayer(true);
	LevelManager::LevelData level_data = LevelManager::sInstance->GetCurrentLevelData();

	LevelLoaderClient loader{};
	m_level_info = loader.LoadLevel(level_data);
}

void WorldClient::Update()
{
	World::Update();

	if (m_client_player != nullptr)
	{
		sf::Vector2f camera_pos = m_camera.getCenter();
		camera_pos.x = m_client_player->GetLocation().mX;
		m_camera.setCenter(camera_pos);
	}

	for (const auto& m_game_object : mGameObjects)
	{
		m_game_object->Update();
	}

	CheckClientCollisions();
	DestroyPlayerOutsideView();
}

void WorldClient::CheckClientCollisions() const
{
	if (m_client_player == nullptr)
		return;

	const sf::FloatRect client_bounds = m_client_player->GetBoundingRect();
	RayGround* client_ray = m_client_player->GetRay();
	const sf::FloatRect client_ray_bounds = client_ray->GetBoundingRect();
	std::set<GameObject::GOPair> ray_collision_pairs;

	for (const auto& go : mGameObjects)
	{
		//Ignore yourself and your ray ground object
		if (go.get() == m_client_player || go.get() == client_ray)
			continue;

		sf::FloatRect go_bounds = go->GetBoundingRect();

		//Character Collisions
		if (go_bounds.intersects(client_bounds))
		{
			//Client Character Collision!
			const GameObject::GOPair collision_pair(go.get(), m_client_player);

			if (CollisionHandler::PlatformCollision(collision_pair,
			                                        [this] { OnReachedCheckpoint(); }, [this]
			                                        {
				                                        OnReachedGoal();
			                                        }, this))
				continue;

			CollisionHandler::TrapCollision(collision_pair, [this] { OnClientPlayerDeath(); });
		}

		//Ray Ground Collisions
		if (go_bounds.intersects(client_ray_bounds))
		{
			//Client Ray Collision!
			GameObject::GOPair collision_pair(go.get(), client_ray);
			CollisionHandler::PlayerGroundRayCast(collision_pair);
		}
	}
}

//Checks if the client player goes outside of the camera bounds
void WorldClient::DestroyPlayerOutsideView() const
{
	if(m_client_player == nullptr)
		return;

	if (!m_client_player->IsDead() && m_client_player->GetLocation().mY > m_camera.getCenter().y + m_camera.getSize().y / 2)
	{
		OnClientPlayerDeath();
	}
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

Character* WorldClient::AddCharacterWithColor(const int player_id, const EColorType color,
                                              const sf::IntRect rect, const Vector3 spawn_pos)
{
	const std::shared_ptr<CharacterClient> player(new CharacterClient(color, rect));
	player->InitRay();
	player->SetLocation(spawn_pos);
	player->SetIdentifier(player_id);

	m_players.emplace_back(player.get());
	AddGameObject(player);

	return player.get();
}

Character* WorldClient::AddCharacter(const int player_id, const EColorType color)
{
	Character* character = AddCharacterWithColor(player_id, color,
	                                             m_level_info.m_red_player_rect,
	                                             m_level_info.m_red_player_spawn_pos);

	m_client_player = dynamic_cast<CharacterClient*>(character);
	UpdateCharacterTransparencies(character->GetTeamIdentifier());

	return character;
}

Character* WorldClient::AddGhostCharacterWithColor(const int identifier, const EColorType color,
                                                   const sf::IntRect& int_rect,
                                                   const Vector3 spawn_pos)
{
	const std::shared_ptr<GhostCharacterClient> ghost_char(
		new GhostCharacterClient(color, int_rect));
	ghost_char->SetLocation(spawn_pos);
	ghost_char->SetIdentifier(identifier);

	m_players.emplace_back(ghost_char.get());
	AddGameObject(ghost_char);
	return ghost_char.get();
}

Character* WorldClient::AddGhostCharacter(const int player_id, const EColorType color)
{
	return AddGhostCharacterWithColor(player_id, color,
	                                  m_level_info.m_blue_player_rect,
	                                  m_level_info.m_blue_player_spawn_pos);
}

Character* WorldClient::GetClientCharacter() const
{
	return m_client_player;
}

void WorldClient::SetTeammate(Character* character)
{
	m_team_mate = character;
}

Character* WorldClient::GetTeammate() const
{
	return m_team_mate;
}

void WorldClient::UpdateCharacterTransparencies(const int team_id) const
{
	for (Character* player : m_players)
	{
		const auto client_char = dynamic_cast<CharacterClient*>(player);
		if (client_char->GetTeamIdentifier() != team_id)
		{
			client_char->SetTransparent();
		}
		else
		{
			client_char->SetOpaque();
		}
	}
}

void WorldClient::UpdateCharacterTransparencies() const
{
	if (GetClientCharacter() != nullptr)
	{
		UpdateCharacterTransparencies(GetClientCharacter()->GetTeamIdentifier());
	}
}

void WorldClient::RespawnClientCharacter() const
{
	//Reset the position of the player to the position of the last checkpoint
	m_client_player->StopMovement();
	const auto& parts = m_checkpoint->GetParts();
	const PlatformPart* part = parts[parts.size() / 2];
	Vector3 position = part->GetLocation();
	position.mY -= static_cast<float>(m_client_player->GetSize().height - 15);
	position.mX += static_cast<float>(part->GetSize().width) / 2.f;
	m_client_player->SetLocation(position);
	m_client_player->SetIsDead(false);

	//Reset all platforms to their initial type
	for (auto& platform : m_level_info.platforms)
	{
		platform->ResetToInitialType();
	}
}

void WorldClient::SetCheckpointToPlatformWithID(const int platform_id)
{
	Platform* new_checkpoint = m_level_info.GetPlatformWithID(platform_id);

	if (new_checkpoint != nullptr)
	{
		m_checkpoint = new_checkpoint;
		m_checkpoint->SetType(EPlatformType::kCheckpointActivated);
	}
}

Character* WorldClient::GetCharacter(const int player_id) const
{
	for (Character* character : m_players)
	{
		if (character->GetPlayerID() == player_id)
		{
			return character;
		}
	}
	return nullptr;
}

void WorldClient::SetPlatformOnCharacter(Character* character, const int platform_id) const
{
	for (const auto& platform : m_level_info.platforms)
	{
		if (platform->GetID() == platform_id)
		{
			character->SetGrounded(platform.get());
			break;
		}
	}
}

void WorldClient::UpdatePlatform(const int id, const int platform_id,
                                 const EPlatformType platform_type)
{
	for (auto& platform : m_level_info.platforms)
	{
		if (platform->GetID() == platform_id)
		{
			if (platform_type == EPlatformType::kCheckpointActivated)
			{
				if (GetClientCharacter()->GetPlayerID() == id)
				{
					platform->SetType(platform_type);
				}
			}
			else
			{
				platform->SetType(platform_type);
			}

			//Initialize the first checkpoint (spawn platform)
			if (m_checkpoint == nullptr)
				m_checkpoint = platform.get();
		}
	}
}

void WorldClient::OnReachedCheckpoint() const
{
	Platform* current_platform = m_client_player->GetCurrentPlatform();

	if (m_team_mate != nullptr && current_platform == m_team_mate->GetCurrentPlatform() &&
		current_platform != m_checkpoint)
		NetworkManagerClient::sInstance->SendCheckpointReached(
			m_client_player->GetTeamIdentifier(), current_platform->GetID());
}

void WorldClient::OnReachedGoal() const
{
	NetworkManagerClient::sInstance->SendGoalReached(GetClientCharacter()->GetTeamIdentifier());
}

void WorldClient::OnClientPlayerDeath() const
{
	m_client_player->SetIsDead(true);
	NetworkManagerClient::sInstance->SendTeamDeath(m_client_player->GetTeamIdentifier());
}
